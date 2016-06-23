#include "LLVMEmitter.h"
#include "EmitterException.h"

namespace emll
{
	namespace compiler
	{
		namespace ir
		{
			llvm::Value* VariableTable::Get(const std::string& name)
			{
				llvm::Value* value = nullptr;
				auto search = _map.find(name);
				if (search != _map.end())
				{
					value = search->second;
				}
				return value;
			}

			void VariableTable::Set(const std::string& name, llvm::Value* pValue)
			{
				_map[name] = pValue;
			}

			LLVMEmitter::LLVMEmitter()
			{
				_pBuilder = std::make_unique<llvm::IRBuilder<>>(_context);
			}
			LLVMEmitter::~LLVMEmitter()
			{

			}

			llvm::Type* LLVMEmitter::Type(ValueType type)
			{
				switch (type)
				{
				case ValueType::Void:
					return GetValueType(type);
				case ValueType::PVoid:
					return GetValueType(ValueType::Void)->getPointerTo();
				case ValueType::Byte:
					return GetValueType(type);
				case ValueType::PByte:
					return GetValueType(ValueType::Byte)->getPointerTo();
				case ValueType::Short:
					return GetValueType(type);
				case ValueType::PShort:
					return GetValueType(ValueType::Short)->getPointerTo();
				case ValueType::Int32:
					return GetValueType(type);
				case ValueType::PInt32:
					return GetValueType(ValueType::Int32)->getPointerTo();
				case ValueType::Int64:
					return GetValueType(type);
				case ValueType::PInt64:
					return GetValueType(ValueType::Int64)->getPointerTo();
				case ValueType::Double:
					return GetValueType(type);
				case ValueType::PDouble:
					return GetValueType(ValueType::Double)->getPointerTo();
				case ValueType::Char8:
					return GetValueType(type);
				case ValueType::PChar8:
					return GetValueType(ValueType::Char8)->getPointerTo();
				default:
					throw new EmitterException(EmitterError::InvalidValueType);
				}
			}

			llvm::Value* LLVMEmitter::Literal(int value)
			{
				return llvm::ConstantInt::get(_context, llvm::APInt(32, value, true));
			}
			llvm::Value* LLVMEmitter::Literal(int64_t value)
			{
				return llvm::ConstantInt::get(_context, llvm::APInt(64, value, true));
			}

			llvm::Value* LLVMEmitter::Literal(double value)
			{
				return llvm::ConstantFP::get(_context, llvm::APFloat(value));
			}

			llvm::Value* LLVMEmitter::Literal(const std::string& value)
			{
				llvm::Value* literal = _stringLiterals.Get(value);
				if (literal == nullptr)
				{
					literal = _pBuilder->CreateGlobalStringPtr(value);
					_stringLiterals.Set(value, literal);
				}
				return literal;
			}

			llvm::Value* LLVMEmitter::Cast(llvm::Value* pValue, ValueType destType)
			{
				return _pBuilder->CreateBitCast(pValue, Type(destType));
			}

			llvm::Value* LLVMEmitter::Global(const std::string& name, const std::string& value)
			{
				return _pBuilder->CreateGlobalStringPtr(value, name);
			}

			llvm::ReturnInst* LLVMEmitter::ReturnVoid()
			{
				return _pBuilder->CreateRetVoid();
			}

			llvm::ReturnInst* LLVMEmitter::Return(llvm::Value* value)
			{
				return _pBuilder->CreateRet(value);
			}

			llvm::Value* LLVMEmitter::BinaryOp(OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal, const std::string& varName)
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

			std::unique_ptr<llvm::Module> LLVMEmitter::AddModule(const std::string& name)
			{
				return std::make_unique<llvm::Module>(name, _context);
			}

			llvm::Function* LLVMEmitter::DeclareFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, ValueTypeList* pArgs)
			{
				return Function(pModule, name, returnType, llvm::Function::LinkageTypes::ExternalLinkage, pArgs);
			}

			llvm::Function* LLVMEmitter::DeclareFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, NamedValueTypeList& args)
			{
				return Function(pModule, name, returnType, llvm::Function::LinkageTypes::ExternalLinkage, args);
			}

			llvm::Function* LLVMEmitter::Function(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, ValueTypeList* pArgs)
			{
				assert(pModule != nullptr);

				llvm::FunctionType* pTypeDef = nullptr;
				if (pArgs != nullptr)
				{
					BindArgTypes(*pArgs);
					pTypeDef = llvm::FunctionType::get(Type(returnType), _argTypes, false);
				}
				else
				{
					pTypeDef = llvm::FunctionType::get(Type(returnType), false);
				}
				return CreateFunction(pModule, name, linkage, pTypeDef);
			}

			llvm::Function* LLVMEmitter::Function(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, NamedValueTypeList& args)
			{
				assert(pModule != nullptr);

				BindArgTypes(args);
				llvm::FunctionType* pTypeDef = llvm::FunctionType::get(Type(returnType), _argTypes, false);
				llvm::Function* pfn = CreateFunction(pModule, name, linkage, pTypeDef);
				BindArgNames(pfn, args);
				return pfn;
			}

			llvm::BasicBlock* LLVMEmitter::Block(llvm::Function* pfn, const std::string& label)
			{
				assert(pfn != nullptr);
				return llvm::BasicBlock::Create(_context, label, pfn);
			}

			llvm::BasicBlock* LLVMEmitter::BlockAfter(llvm::Function* pfn, llvm::BasicBlock* pPrevBlock, const std::string& label)
			{
				assert(pfn != nullptr);
				assert(pPrevBlock != nullptr);

				llvm::BasicBlock* newBlock = Block(label);
				pfn->getBasicBlockList().insertAfter(pPrevBlock->getIterator(), newBlock);
				return newBlock;
			}

			llvm::BasicBlock* LLVMEmitter::Block(const std::string& label)
			{
				return llvm::BasicBlock::Create(_context, label);
			}

			void LLVMEmitter::SetCurrentBlock(llvm::BasicBlock* pBlock)
			{
				assert(pBlock != nullptr);
				_pBuilder->SetInsertPoint(pBlock);
			}

			llvm::CallInst* LLVMEmitter::Call(llvm::Function* pfn)
			{
				assert(pfn != nullptr);
				return _pBuilder->CreateCall(pfn, llvm::None);
			}

			llvm::CallInst* LLVMEmitter::Call(llvm::Function* pfn, llvm::Value* pArg)
			{
				assert(pfn != nullptr);
				return _pBuilder->CreateCall(pfn, pArg);
			}

			llvm::CallInst* LLVMEmitter::Call(llvm::Function* pfn, ValueList& args)
			{
				assert(pfn != nullptr);
				return _pBuilder->CreateCall(pfn, args);
			}

			llvm::PHINode* LLVMEmitter::Phi(ValueType type, llvm::Value* pLVal, llvm::BasicBlock* pLBlock, llvm::Value* pRVal, llvm::BasicBlock* pRBlock)
			{
				llvm::PHINode* phi = _pBuilder->CreatePHI(Type(type), 2);
				phi->addIncoming(pLVal, pLBlock);
				phi->addIncoming(pRVal, pRBlock);
				return phi;
			}

			llvm::AllocaInst* LLVMEmitter::Variable(ValueType type)
			{
				return _pBuilder->CreateAlloca(Type(type), nullptr);
			}

			llvm::AllocaInst* LLVMEmitter::Variable(ValueType type, const std::string& name)
			{
				return _pBuilder->CreateAlloca(Type(type), nullptr, name);
			}

			llvm::AllocaInst* LLVMEmitter::Variable(llvm::Type* pType, const std::string& name)
			{
				return _pBuilder->CreateAlloca(pType, nullptr, name);
			}

			llvm::AllocaInst* LLVMEmitter::StackAlloc(ValueType type, int size)
			{
				return _pBuilder->CreateAlloca(Type(type), Literal(size));
			}

			llvm::Value* LLVMEmitter::Cmp(ComparisonType type, llvm::Value* pLVal, llvm::Value* pRVal)
			{
				switch (type)
				{
				default:
					throw new EmitterException(EmitterError::InvalidComparisonType);
				case ComparisonType::Eq:
					return _pBuilder->CreateICmpEQ(pLVal, pRVal);
				case ComparisonType::Lt:
					return _pBuilder->CreateICmpSLT(pLVal, pRVal);
				case ComparisonType::Lte:
					return _pBuilder->CreateICmpSLE(pLVal, pRVal);
				case ComparisonType::Gt:
					return _pBuilder->CreateICmpSGT(pLVal, pRVal);
				case ComparisonType::Gte:
					return _pBuilder->CreateICmpSGE(pLVal, pRVal);
				case ComparisonType::Neq:
					return _pBuilder->CreateICmpNE(pLVal, pRVal);
				}
			}

			llvm::BranchInst* LLVMEmitter::Branch(llvm::Value* pCondVal, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
			{
				return _pBuilder->CreateCondBr(pCondVal, pThenBlock, pElseBlock);
			}

			llvm::BranchInst* LLVMEmitter::Branch(llvm::BasicBlock* pDest)
			{
				return _pBuilder->CreateBr(pDest);
			}

			llvm::Type* LLVMEmitter::GetValueType(ValueType type)
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

			void LLVMEmitter::BindArgTypes(ValueTypeList& args)
			{
				_argTypes.clear();
				for (auto arg = args.begin(); arg != args.end(); ++arg)
				{
					_argTypes.push_back(Type(*arg));
				}
			}

			void LLVMEmitter::BindArgTypes(NamedValueTypeList& args)
			{
				_argTypes.clear();
				for (auto arg = args.begin(); arg != args.end(); ++arg)
				{
					_argTypes.push_back(Type(arg->second));
				}
			}

			void LLVMEmitter::BindArgNames(llvm::Function* pfn, NamedValueTypeList& args)
			{
				size_t i = 0;
				for (auto &arg : pfn->args())
				{
					arg.setName(args[i++].first);
				}
			}

			llvm::Function* LLVMEmitter::CreateFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pTypeDef)
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
}