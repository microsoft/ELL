#include "stdafx.h"
#include "LLVMEmitter.h"
#include "EmitterException.h"

namespace emll
{
	namespace codegen
	{
		LLVMEmitter::LLVMEmitter()
		{
			_pBuilder = std::make_unique<llvm::IRBuilder<>>(_context);
		}
		LLVMEmitter::~LLVMEmitter()
		{

		}

		llvm::Type* LLVMEmitter::valueType(ValueType type)
		{
			switch (type)
			{
			case ValueType::Void:
				return getValueType(type);
			case ValueType::PVoid:
				return getValueType(ValueType::Void)->getPointerTo();
			case ValueType::Byte:
				return getValueType(type);
			case ValueType::PByte:
				return getValueType(ValueType::Byte)->getPointerTo();
			case ValueType::Short:
				return getValueType(type);
			case ValueType::PShort:
				return getValueType(ValueType::Short)->getPointerTo();
			case ValueType::Int32:
				return getValueType(type);
			case ValueType::PInt32:
				return getValueType(ValueType::Int32)->getPointerTo();
			case ValueType::Int64:
				return getValueType(type);
			case ValueType::PInt64:
				return getValueType(ValueType::Int64)->getPointerTo();
			case ValueType::Double:
				return getValueType(type);
			case ValueType::PDouble:
				return getValueType(ValueType::Double)->getPointerTo();
			case ValueType::Char8:
				return getValueType(type);
			case ValueType::PChar8:
				return getValueType(ValueType::Char8)->getPointerTo();
			default:
				throw new EmitterException(EmitterError::InvalidValueType);
			}
		}

		llvm::Value* LLVMEmitter::literal(int value)
		{
			return llvm::ConstantInt::get(_context, llvm::APInt(32, value, true));
		}
		llvm::Value* LLVMEmitter::literal(int64_t value)
		{
			return llvm::ConstantInt::get(_context, llvm::APInt(64, value, true));
		}

		llvm::Value* LLVMEmitter::literal(double value)
		{
			return llvm::ConstantFP::get(_context, llvm::APFloat(value));
		}

		llvm::Value* LLVMEmitter::literal(const std::string& value)
		{
			llvm::Value* literal = nullptr;
			auto search = _stringLiterals.find(value);
			if (search != _stringLiterals.end())
			{
				literal = search->second;
			}
			else
			{
				literal = _pBuilder->CreateGlobalStringPtr(value);
				_stringLiterals[value] = literal;
			}

			return literal;
		}

		llvm::Value* LLVMEmitter::addCast(llvm::Value* pValue, ValueType destType)
		{
			return _pBuilder->CreateBitCast(pValue, valueType(destType));
		}

		llvm::Value* LLVMEmitter::addGlobal(const std::string& name, const std::string& value)
		{
			return _pBuilder->CreateGlobalStringPtr(value, name);
		}

		llvm::ReturnInst* LLVMEmitter::addReturnVoid()
		{
			return _pBuilder->CreateRetVoid();
		}

		llvm::ReturnInst* LLVMEmitter::addReturn(llvm::Value* value)
		{
			return _pBuilder->CreateRet(value);
		}

		llvm::Value* LLVMEmitter::addBinaryOp(OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal, const std::string& varName)
		{
			switch (type)
			{
			case OperatorType::Add:
				return _pBuilder->CreateAdd(pLVal, pRVal, varName);
			case OperatorType::Subtract:
				return _pBuilder->CreateSub(pLVal, pRVal, varName);
			case OperatorType::Multiply:
				return _pBuilder->CreateMul(pLVal, pRVal, varName);
			case OperatorType::DivideS:
				return _pBuilder->CreateSDiv(pLVal, pRVal, varName);
			case OperatorType::AddF:
				return _pBuilder->CreateFAdd(pLVal, pRVal, varName);
			case OperatorType::SubtractF:
				return _pBuilder->CreateFSub(pLVal, pRVal, varName);
			case OperatorType::MultiplyF:
				return _pBuilder->CreateFMul(pLVal, pRVal, varName);
			case OperatorType::DivideF:
				return _pBuilder->CreateFDiv(pLVal, pRVal, varName);
			default:
				throw new EmitterException(EmitterError::InvalidOperatorType);
			}
		}

		std::unique_ptr<llvm::Module> LLVMEmitter::addModule(const std::string& name)
		{
			return std::make_unique<llvm::Module>(name, _context);
		}

		llvm::Function* LLVMEmitter::addDeclaration(llvm::Module* pModule, const std::string& name, ValueType returnType, ValueTypeList* pArgs)
		{
			return addFunction(pModule, name, returnType, llvm::Function::LinkageTypes::ExternalLinkage, pArgs);
		}

		llvm::Function* LLVMEmitter::addDeclaration(llvm::Module* pModule, const std::string& name, ValueType returnType, NamedValueTypeList& args)
		{
			return addFunction(pModule, name, returnType, llvm::Function::LinkageTypes::ExternalLinkage, args);
		}

		llvm::Function* LLVMEmitter::addFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, ValueTypeList* pArgs)
		{
			assert(pModule != nullptr);

			llvm::FunctionType* pTypeDef = nullptr;
			if (pArgs != nullptr)
			{
				bindArgTypes(*pArgs);
				pTypeDef = llvm::FunctionType::get(valueType(returnType), _argTypes, false);
			}
			else
			{
				pTypeDef = llvm::FunctionType::get(valueType(returnType), false);
			}
			return createFunction(pModule, name, linkage, pTypeDef);
		}

		llvm::Function* LLVMEmitter::addFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, NamedValueTypeList& args)
		{
			assert(pModule != nullptr);

			bindArgTypes(args);
			llvm::FunctionType* pTypeDef = llvm::FunctionType::get(valueType(returnType), _argTypes, false);
			llvm::Function* pfn = createFunction(pModule, name, linkage, pTypeDef);
			bindArgNames(pfn, args);
			return pfn;
		}

		llvm::BasicBlock* LLVMEmitter::addBlock(llvm::Function* pfn, const std::string& label)
		{
			assert(pfn != nullptr);
			return llvm::BasicBlock::Create(_context, label, pfn);
		}

		void LLVMEmitter::setCurrentBlock(llvm::BasicBlock* pBlock)
		{
			assert(pBlock != nullptr);
			_pBuilder->SetInsertPoint(pBlock);
		}

		llvm::CallInst* LLVMEmitter::addCall(llvm::Function* pfn)
		{
			assert(pfn != nullptr);
			return _pBuilder->CreateCall(pfn, llvm::None);
		}

		llvm::CallInst* LLVMEmitter::addCall(llvm::Function* pfn, llvm::Value* pArg)
		{
			assert(pfn != nullptr);
			return _pBuilder->CreateCall(pfn, pArg);
		}

		llvm::CallInst* LLVMEmitter::addCall(llvm::Function* pfn, ValueList& args)
		{
			assert(pfn != nullptr);
			return _pBuilder->CreateCall(pfn, args);
		}

		llvm::PHINode* LLVMEmitter::addPhi(ValueType type, llvm::Value* pLVal, llvm::BasicBlock* pLBlock, llvm::Value* pRVal, llvm::BasicBlock* pRBlock)
		{
			llvm::PHINode* phi =_pBuilder->CreatePHI(valueType(type), 2);
			phi->addIncoming(pLVal, pLBlock);
			phi->addIncoming(pRVal, pRBlock);
			return phi;
		}

		llvm::AllocaInst* LLVMEmitter::addVariable(ValueType type)
		{
			return _pBuilder->CreateAlloca(valueType(type), nullptr);
		}

		llvm::AllocaInst* LLVMEmitter::addStackAlloc(ValueType type, int size)
		{
			return _pBuilder->CreateAlloca(valueType(type), literal(size));
		}

		llvm::Type* LLVMEmitter::getValueType(ValueType type)
		{
			switch (type)
			{
			case ValueType::Void:
				return _pBuilder->getVoidTy();
			case ValueType::Byte:
				return _pBuilder->getInt8Ty();
			case ValueType::Short:
				return _pBuilder->getInt16Ty();
			case ValueType::Int32:
				return _pBuilder->getInt32Ty();
			case ValueType::Int64:
				return _pBuilder->getInt64Ty();
			case ValueType::Double:
				return _pBuilder->getDoubleTy();
			case ValueType::Char8:
				return _pBuilder->getInt8Ty();
			default:
				throw new EmitterException(EmitterError::InvalidValueType);
			}
		}
		
		void LLVMEmitter::bindArgTypes(ValueTypeList& args)
		{
			_argTypes.clear();
			for (auto arg = args.begin(); arg != args.end(); ++arg)
			{
				_argTypes.push_back(valueType(*arg));
			}
		}

		void LLVMEmitter::bindArgTypes(NamedValueTypeList& args)
		{
			_argTypes.clear();
			for (auto arg = args.begin(); arg != args.end(); ++arg)
			{
				_argTypes.push_back(valueType(arg->second));
			}
		}

		void LLVMEmitter::bindArgNames(llvm::Function* pfn, NamedValueTypeList& args)
		{
			size_t i = 0;
			for (auto &arg : pfn->args())
			{
				arg.setName(args[i++].first);
			}
		}

		llvm::Function* LLVMEmitter::createFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pTypeDef)
		{
			llvm::Function* pfn = llvm::Function::Create(pTypeDef, linkage, name, pModule);
			if (pfn == nullptr)
			{
				throw new EmitterException(EmitterError::InvalidFunction);
			}
			return pfn;
		}
	}
}