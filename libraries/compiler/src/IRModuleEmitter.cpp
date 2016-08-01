#include "CompilerException.h"
#include "IRModuleEmitter.h"

namespace emll
{
	namespace compiler
	{
		IRModuleEmitter::IRModuleEmitter(IREmitter& emitter, const std::string& name)
			: IRModuleEmitter(emitter, emitter.AddModule(name))
		{
		}

		IRModuleEmitter::IRModuleEmitter(IREmitter& emitter, std::unique_ptr<llvm::Module> pModule)
			: _emitter(emitter), 
			_pModule(std::move(pModule))
		{
		}

		llvm::GlobalVariable* IRModuleEmitter::Constant(const ValueType type, const std::string& name, double value)
		{
			return Global(name, _emitter.Type(type), _emitter.Literal(value), true);
		}

		llvm::GlobalVariable* IRModuleEmitter::Constant(const std::string& name, const std::vector<double>& value)
		{
			return Global(name, _emitter.ArrayType(ValueType::Double, value.size()), _emitter.Literal(value), true);
		}

		llvm::GlobalVariable* IRModuleEmitter::Global(const ValueType type, const std::string& name)
		{
			return Global(name, _emitter.Type(type), _emitter.Zero(type), false);
		}

		llvm::GlobalVariable* IRModuleEmitter::Global(const ValueType type, const std::string& name, const uint64_t size)
		{
			llvm::ArrayType* pArrayType = _emitter.ArrayType(type, size);
			return Global(name, pArrayType, InitializeArray(pArrayType), false);
		}

		llvm::GlobalVariable* IRModuleEmitter::Global(const std::string& name, llvm::Type* pType, const uint64_t size)
		{
			assert(pType != nullptr);

			llvm::ArrayType* pArrayType = llvm::ArrayType::get(pType, size);
			return Global(name, pArrayType, InitializeArray(pArrayType), false);
		}

		llvm::GlobalVariable* IRModuleEmitter::Global(const std::string&name, const std::vector<double>& value)
		{
			return Global(name, _emitter.ArrayType(ValueType::Double, value.size()), _emitter.Literal(value), false);
		}

		llvm::StructType* IRModuleEmitter::Struct(const std::string& name, std::initializer_list<ValueType> fields)
		{
			_valueTypeList.init(fields);
			return _emitter.Struct(name, _valueTypeList);
		}

		IRFunctionEmitter IRModuleEmitter::AddMain()
		{
			return Function("main", ValueType::Void, true);
		}

		IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, const ValueType returnType, const NamedValueTypeList& args, bool isPublic)
		{
			llvm::Function* pfn = _emitter.Function(Module(), name, returnType, Linkage(isPublic), args);
			if (pfn == nullptr)
			{
				throw new CompilerException(CompilerError::functionNotFound);
			}
			BeginFunction(pfn);
			return IRFunctionEmitter(&_emitter, pfn);
		}

		IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, const ValueType returnType, const std::initializer_list<ValueType> args, bool isPublic)
		{
			_valueTypeList.init(args);
			return Function(name, returnType, &_valueTypeList, isPublic);
		}

		llvm::GlobalVariable* IRModuleEmitter::Global(const std::string& name, llvm::Type *pType, llvm::Constant* pInitial, bool isConst)
		{
			return new llvm::GlobalVariable(*_pModule, pType, isConst, llvm::GlobalValue::InternalLinkage, pInitial, name);
		}

		llvm::Function* IRModuleEmitter::GetFunction(const std::string& name)
		{
			return _pModule->getFunction(name);
		}

		llvm::FunctionPassManager* IRModuleEmitter::FunctionOptimizer()
		{
			if (_pOptimizer == nullptr)
			{
				_pOptimizer = std::make_unique<llvm::FunctionPassManager>(_pModule.get());
				/*
				_pOptimizer->addPass(llvm::createInstructionCombiningPass());
				_pOptimizer->addPass(llvm::createReassociatePass());
				_pOptimizer->addPass(llvm::createGVNPass());  // Common sub-expression elimination
				_pOptimizer->addPass(llvm::createCFGSimplificationPass());
				*/
			}
			return _pOptimizer.get();
		}

		IRFunctionEmitter IRModuleEmitter::Function(const std::string& name, const ValueType returnType, const ValueTypeList* pArgs, bool isPublic)
		{
			llvm::Function* pfn = _emitter.Function(Module(), name, returnType, Linkage(isPublic), pArgs);
			if (pfn == nullptr)
			{
				throw new CompilerException(CompilerError::functionNotFound);
			}
			BeginFunction(pfn);
			return IRFunctionEmitter(&_emitter, pfn);
		}

		llvm::Function::LinkageTypes IRModuleEmitter::Linkage(bool isPublic)
		{
			return isPublic ?
				llvm::Function::LinkageTypes::ExternalLinkage :
				llvm::Function::LinkageTypes::InternalLinkage;
		}

		llvm::ConstantAggregateZero* IRModuleEmitter::InitializeArray(llvm::ArrayType* pType)
		{
			assert(pType != nullptr);
			return llvm::ConstantAggregateZero::get(pType);
		}

		void IRModuleEmitter::BeginFunction(llvm::Function* pfn)
		{
			//
			// Set us up with a default entry point, since we'll always need one
			// We may add additional annotations here
			//
			auto pBlock = _emitter.Block(pfn, "entry");
			_emitter.SetCurrentBlock(pBlock);
		}

		void IRModuleEmitter::WriteToFile(const std::string& filePath, bool isBitCode)
		{
			std::error_code error;

			auto openFlags = isBitCode ?
				llvm::sys::fs::F_None :
				llvm::sys::fs::F_Text;
			llvm::tool_output_file out(filePath, error, openFlags);
			if (error) {
				throw new LLVMException(error);
			}

			if (isBitCode)
			{
				llvm::WriteBitcodeToFile(_pModule.get(), out.os());
			}
			else
			{
				Module()->print(out.os(), nullptr);
			}
			if (out.os().has_error()) {
				throw new CompilerException(CompilerError::writeStreamFailed);
			}
			out.keep();
		}

		void IRModuleEmitter::WriteToStream(std::ostream& os, bool isBitCode)
		{
			llvm::raw_os_ostream out(os);
			if (isBitCode)
			{
				llvm::WriteBitcodeToFile(_pModule.get(), out);
			}
			else
			{
				Module()->print(out, nullptr);
			}
		}
		//
		// Standard C Runtime functions
		//
		void IRModuleEmitter::DeclarePrintf()
		{
			llvm::FunctionType* type = llvm::TypeBuilder<void(char*, ...), false>::get(_emitter.Context());
			DeclareFunction("printf", type);
		}
		void IRModuleEmitter::DeclareMalloc()
		{
			_valueTypeList.init({ ValueType::Int64 });
			DeclareFunction("malloc", ValueType::PByte, _valueTypeList);
		}
		void IRModuleEmitter::DeclareFree()
		{
			_valueTypeList.init({ ValueType::PByte });
			DeclareFunction("free", ValueType::Void, _valueTypeList);
		}
	}
}