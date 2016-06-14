#include "stdafx.h"
#include "EmitterException.h"
#include "ModuleEmitter.h"

namespace emll
{
	namespace codegen
	{
		ModuleEmitter::ModuleEmitter(LLVMEmitter* pEmitter, std::unique_ptr<llvm::Module> pModule)
			: _pEmitter(pEmitter),
			_pModule(std::move(pModule))
		{
		}
		ModuleEmitter::~ModuleEmitter()
		{
		}

		FunctionEmitter ModuleEmitter::addMain()
		{
			return function("main", codegen::ValueType::Void, true);
		}

		llvm::Function* ModuleEmitter::getFunction(const std::string& name)
		{
			return _pModule->getFunction(name);
		}

		FunctionEmitter ModuleEmitter::function(const std::string& name, ValueType returnType, ValueTypeList* pArgs, bool isPublic)
		{
			llvm::Function* pfn = _pEmitter->addFunction(module(), name, returnType, linkage(isPublic), pArgs);
			if (pfn == nullptr)
			{
				throw new EmitterException(EmitterError::InvalidFunction);
			}
			beginFunction(pfn);
			return FunctionEmitter(_pEmitter, pfn);
		}

		FunctionEmitter ModuleEmitter::function(const std::string& name, ValueType returnType, std::initializer_list<ValueType> args, bool isPublic)
		{
			_valueTypeList.init(args);
			return function(name, returnType, &_valueTypeList, isPublic);
		}

		llvm::Function::LinkageTypes ModuleEmitter::linkage(bool isPublic)
		{
			return isPublic ?
				llvm::Function::LinkageTypes::ExternalLinkage :
				llvm::Function::LinkageTypes::InternalLinkage;
		}

		void ModuleEmitter::beginFunction(llvm::Function* pfn)
		{
			//
			// Set us up with a default entry point, since we'll always need one
			// We may add additional annotations here
			//
			auto pBlock = _pEmitter->addBlock(pfn, "entry");
			_pEmitter->setCurrentBlock(pBlock);
		}

		void ModuleEmitter::writeToFile(const std::string& filePath, bool isBitCode) const
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
				module()->print(out.os(), nullptr);
			}
			if (out.os().has_error()) {
				throw new EmitterException(EmitterError::WriteModuleFailed);
			}
			out.keep();
		}
		
		//
		// Standard C Runtime functions
		//
		void ModuleEmitter::declarePrintf()
		{
			llvm::FunctionType* type = llvm::TypeBuilder<void(char*, ...), false>::get(_pEmitter->context());
			declareFunction("printf", type);
		}
		void ModuleEmitter::declareMalloc()
		{
			_valueTypeList.init({ ValueType::Int64 });
			declareFunction("malloc", ValueType::PByte, _valueTypeList);
		}
		void ModuleEmitter::declareFree()
		{
			_valueTypeList.init({ ValueType::PByte});
			declareFunction("free", ValueType::Void, _valueTypeList);
		}
	}
}