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
				return _pBuilder->CreateBitCast(pValue, valueType(destType));
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
					bindArgTypes(*pArgs);
					pTypeDef = llvm::FunctionType::get(valueType(returnType), _argTypes, false);
				}
				else
				{
					pTypeDef = llvm::FunctionType::get(valueType(returnType), false);
				}
				return createFunction(pModule, name, linkage, pTypeDef);
			}

			llvm::Function* LLVMEmitter::Function(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, NamedValueTypeList& args)
			{
				assert(pModule != nullptr);

				bindArgTypes(args);
				llvm::FunctionType* pTypeDef = llvm::FunctionType::get(valueType(returnType), _argTypes, false);
				llvm::Function* pfn = createFunction(pModule, name, linkage, pTypeDef);
				bindArgNames(pfn, args);
				return pfn;
			}

			llvm::BasicBlock* LLVMEmitter::Block(llvm::Function* pfn, const std::string& label)
			{
				assert(pfn != nullptr);
				return llvm::BasicBlock::Create(_context, label, pfn);
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
				llvm::PHINode* phi = _pBuilder->CreatePHI(valueType(type), 2);
				phi->addIncoming(pLVal, pLBlock);
				phi->addIncoming(pRVal, pRBlock);
				return phi;
			}

			llvm::AllocaInst* LLVMEmitter::Variable(ValueType type)
			{
				return _pBuilder->CreateAlloca(valueType(type), nullptr);
			}

			llvm::AllocaInst* LLVMEmitter::Variable(ValueType type, const std::string& name)
			{
				return _pBuilder->CreateAlloca(valueType(type), nullptr, name);
			}

			llvm::AllocaInst* LLVMEmitter::StackAlloc(ValueType type, int size)
			{
				return _pBuilder->CreateAlloca(valueType(type), Literal(size));
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
}