#include "IREmitter.h"
#include "EmitterException.h"

namespace emll
{
	namespace compiler
	{
		IREmitter::IREmitter()
			: _builder(_context)
		{
		}

		llvm::Type* IREmitter::Type(const ValueType type)
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

		llvm::ArrayType* IREmitter::ArrayType(const ValueType type, uint64_t size)
		{
			return llvm::ArrayType::get(Type(type), size);
		}

		llvm::Constant* IREmitter::Literal(const unsigned char value)
		{
			return Integer(ValueType::Byte, value);
		}

		llvm::Constant* IREmitter::Literal(const short value)
		{
			return Integer(ValueType::Short, value);
		}

		llvm::Constant* IREmitter::Literal(const int value)
		{
			return Integer(ValueType::Int32, value);
		}

		llvm::Constant* IREmitter::Literal(const int64_t value)
		{
			return Integer(ValueType::Int64, value);
		}

		llvm::Constant* IREmitter::Literal(const double value)
		{
			return llvm::ConstantFP::get(_context, llvm::APFloat(value));
		}

		llvm::Value* IREmitter::Literal(const std::string& value)
		{
			llvm::Value* literal = _stringLiterals.Get(value);
			if (literal == nullptr)
			{
				literal = _builder.CreateGlobalStringPtr(value);
				_stringLiterals.Set(value, literal);
			}
			return literal;
		}

		llvm::Constant* IREmitter::Literal(const std::vector<double>& value)
		{
			return llvm::ConstantDataArray::get(_context, value);
		}

		llvm::Value* IREmitter::Literal(const std::string& name, const std::string& value)
		{
			return _builder.CreateGlobalStringPtr(value, name);
		}

		llvm::Constant* IREmitter::Zero(ValueType type)
		{
			switch (type)
			{
				case ValueType::Byte:
				case ValueType::Short:
				case ValueType::Int32:
				case ValueType::Int64:
					return Integer(type, 0);
				case ValueType::Double:
					return Literal(0.0);
				default:
					break;
			}
			return nullptr;
		}

		llvm::Value* IREmitter::Cast(llvm::Value* pValue, const ValueType destType)
		{
			assert(pValue != nullptr);

			return _builder.CreateBitCast(pValue, Type(destType));
		}

		llvm::Value* IREmitter::CastFloat(llvm::Value* pValue, const ValueType destType)
		{
			assert(pValue != nullptr);

			auto type = Type(destType);
			switch (destType)
			{
				case ValueType::Byte:
					return _builder.CreateFPToUI(pValue, type);

				case ValueType::Short:
				case ValueType::Int32:
				case ValueType::Int64:
					return _builder.CreateFPToSI(pValue, type);
				
				default:
					throw new EmitterException(EmitterError::NotSupported);
			}
		}

		llvm::ReturnInst* IREmitter::ReturnVoid()
		{
			return _builder.CreateRetVoid();
		}

		llvm::ReturnInst* IREmitter::Return(llvm::Value* pValue)
		{
			assert(pValue != nullptr);
			return _builder.CreateRet(pValue);
		}

		llvm::Value* IREmitter::BinaryOp(const OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal, const std::string& varName)
		{
			assert(pLVal != nullptr);
			assert(pRVal != nullptr);

			switch (type)
			{
				case OperatorType::Add:
					return _builder.CreateAdd(pLVal, pRVal, varName);
				case OperatorType::Subtract:
					return _builder.CreateSub(pLVal, pRVal, varName);
				case OperatorType::Multiply:
					return _builder.CreateMul(pLVal, pRVal, varName);
				case OperatorType::DivideS:
					return _builder.CreateSDiv(pLVal, pRVal, varName);
				case OperatorType::AddF:
					return _builder.CreateFAdd(pLVal, pRVal, varName);
				case OperatorType::SubtractF:
					return _builder.CreateFSub(pLVal, pRVal, varName);
				case OperatorType::MultiplyF:
					return _builder.CreateFMul(pLVal, pRVal, varName);
				case OperatorType::DivideF:
					return _builder.CreateFDiv(pLVal, pRVal, varName);
				default:
					throw new EmitterException(EmitterError::InvalidOperatorType);
			}
		}

		llvm::Value* IREmitter::Cmp(const ComparisonType type, llvm::Value* pLVal, llvm::Value* pRVal)
		{
			assert(pLVal != nullptr);
			assert(pRVal != nullptr);

			switch (type)
			{
				default:
					throw new EmitterException(EmitterError::InvalidComparisonType);
				case ComparisonType::Eq:
					return _builder.CreateICmpEQ(pLVal, pRVal);
				case ComparisonType::Lt:
					return _builder.CreateICmpSLT(pLVal, pRVal);
				case ComparisonType::Lte:
					return _builder.CreateICmpSLE(pLVal, pRVal);
				case ComparisonType::Gt:
					return _builder.CreateICmpSGT(pLVal, pRVal);
				case ComparisonType::Gte:
					return _builder.CreateICmpSGE(pLVal, pRVal);
				case ComparisonType::Neq:
					return _builder.CreateICmpNE(pLVal, pRVal);
				case ComparisonType::EqF:
					return _builder.CreateFCmpOEQ(pLVal, pRVal);
				case ComparisonType::LtF:
					return _builder.CreateFCmpOLT(pLVal, pRVal);
				case ComparisonType::LteF:
					return _builder.CreateFCmpOLE(pLVal, pRVal);
				case ComparisonType::GtF:
					return _builder.CreateFCmpOGT(pLVal, pRVal);
				case ComparisonType::GteF:
					return _builder.CreateFCmpOGE(pLVal, pRVal);
			}
		}

		std::unique_ptr<llvm::Module> IREmitter::AddModule(const std::string& name)
		{
			return std::make_unique<llvm::Module>(name, _context);
		}

		llvm::Function* IREmitter::DeclareFunction(llvm::Module* pModule, const std::string& name, const ValueType returnType, const ValueTypeList* pArgs)
		{
			return Function(pModule, name, returnType, llvm::Function::LinkageTypes::ExternalLinkage, pArgs);
		}

		llvm::Function* IREmitter::DeclareFunction(llvm::Module* pModule, const std::string& name, ValueType returnType, const NamedValueTypeList& args)
		{
			return Function(pModule, name, returnType, llvm::Function::LinkageTypes::ExternalLinkage, args);
		}

		llvm::Function* IREmitter::DeclareFunction(llvm::Module* pModule, const std::string& name, llvm::FunctionType* type)
		{
			assert(pModule != nullptr);

			return static_cast<llvm::Function*>(pModule->getOrInsertFunction(name, type));
		}

		llvm::Function* IREmitter::Function(llvm::Module* pModule, const std::string& name, ValueType returnType, llvm::Function::LinkageTypes linkage, const ValueTypeList* pArgs)
		{
			assert(pModule != nullptr);

			llvm::FunctionType* pTypeDef = nullptr;
			if (pArgs != nullptr)
			{
				BindTypes(*pArgs);
				pTypeDef = llvm::FunctionType::get(Type(returnType), _types, false);
			}
			else
			{
				pTypeDef = llvm::FunctionType::get(Type(returnType), false);
			}
			return CreateFunction(pModule, name, linkage, pTypeDef);
		}

		llvm::Function* IREmitter::Function(llvm::Module* pModule, const std::string& name, const ValueType returnType, llvm::Function::LinkageTypes linkage, const NamedValueTypeList& args)
		{
			assert(pModule != nullptr);

			BindArgTypes(args);
			llvm::FunctionType* pTypeDef = llvm::FunctionType::get(Type(returnType), _types, false);
			llvm::Function* pfn = CreateFunction(pModule, name, linkage, pTypeDef);
			BindArgNames(pfn, args);
			return pfn;
		}

		llvm::BasicBlock* IREmitter::Block(llvm::Function* pfn, const std::string& label)
		{
			assert(pfn != nullptr);
			return llvm::BasicBlock::Create(_context, label, pfn);
		}

		llvm::BasicBlock* IREmitter::BlockAfter(llvm::Function* pfn, llvm::BasicBlock* pPrevBlock, const std::string& label)
		{
			assert(pfn != nullptr);
			assert(pPrevBlock != nullptr);

			llvm::BasicBlock* newBlock = Block(label);
			pfn->getBasicBlockList().insertAfter(pPrevBlock->getIterator(), newBlock);
			return newBlock;
		}

		llvm::BasicBlock* IREmitter::Block(const std::string& label)
		{
			return llvm::BasicBlock::Create(_context, label);
		}

		void IREmitter::SetCurrentBlock(llvm::BasicBlock* pBlock)
		{
			assert(pBlock != nullptr);
			_builder.SetInsertPoint(pBlock);
		}

		llvm::CallInst* IREmitter::Call(llvm::Function* pfn)
		{
			assert(pfn != nullptr);
			return _builder.CreateCall(pfn, llvm::None);
		}

		llvm::CallInst* IREmitter::Call(llvm::Function* pfn, llvm::Value* pArg)
		{
			assert(pfn != nullptr);
			return _builder.CreateCall(pfn, pArg);
		}

		llvm::CallInst* IREmitter::Call(llvm::Function* pfn, const IRValueList& args)
		{
			assert(pfn != nullptr);
			return _builder.CreateCall(pfn, args);
		}

		llvm::CallInst* IREmitter::MemMove(llvm::Value* pSrc, llvm::Value* pDest, llvm::Value* pCount)
		{
			assert(pSrc != nullptr);
			assert(pDest != nullptr);
			assert(pCount != nullptr);
			return _builder.CreateMemMove(pDest, pSrc, pCount, 8);
		}

		llvm::PHINode* IREmitter::Phi(const ValueType type, llvm::Value* pLVal, llvm::BasicBlock* pLBlock, llvm::Value* pRVal, llvm::BasicBlock* pRBlock)
		{
			assert(pLBlock != nullptr);
			assert(pRBlock != nullptr);
			assert(pRVal != nullptr);
			assert(pRBlock != nullptr);

			llvm::PHINode* phi = _builder.CreatePHI(Type(type), 2);
			phi->addIncoming(pLVal, pLBlock);
			phi->addIncoming(pRVal, pRBlock);
			return phi;
		}

		llvm::Value* IREmitter::PtrOffset(llvm::Value* pArray, llvm::Value* pOffset)
		{
			assert(pArray != nullptr);
			assert(pOffset != nullptr);
			return _builder.CreateGEP(pArray, pOffset);
		}

		llvm::Value* IREmitter::GlobalPtrOffset(llvm::GlobalVariable* pArray, llvm::Value* pOffset)
		{
			assert(pArray != nullptr);
			assert(pOffset != nullptr);

			llvm::Value* derefArgs[2]{
				Zero(),
				pOffset
			};
			return _builder.CreateInBoundsGEP(pArray->getValueType(), pArray, derefArgs);
		}

		llvm::Value* IREmitter::GlobalPtrOffset(llvm::GlobalVariable* pArray, llvm::Value* pOffset, llvm::Value* pFieldOffset)
		{
			assert(pArray != nullptr);
			assert(pOffset != nullptr);
			assert(pFieldOffset != nullptr);
			
			llvm::Value* derefArgs[3]{
				Zero(),
				pOffset, 
				pFieldOffset
			};
			return _builder.CreateInBoundsGEP(pArray->getValueType(), pArray, derefArgs);
		}

		llvm::LoadInst* IREmitter::Load(llvm::Value* pPtr)
		{
			assert(pPtr != nullptr);
			return _builder.CreateLoad(pPtr);
		}

		llvm::StoreInst* IREmitter::Store(llvm::Value* pPtr, llvm::Value* pVal)
		{
			assert(pPtr != nullptr);
			assert(pVal != nullptr);
			return _builder.CreateStore(pVal, pPtr);
		}

		llvm::AllocaInst* IREmitter::Variable(const ValueType type)
		{
			return _builder.CreateAlloca(Type(type), nullptr);
		}

		llvm::AllocaInst* IREmitter::Variable(const ValueType type, const std::string& name)
		{
			return _builder.CreateAlloca(Type(type), nullptr, name);
		}

		llvm::AllocaInst* IREmitter::Variable(llvm::Type* pType, const std::string& name)
		{
			assert(pType != nullptr);

			return _builder.CreateAlloca(pType, nullptr, name);
		}

		llvm::AllocaInst* IREmitter::StackAlloc(const ValueType type, int size)
		{
			return _builder.CreateAlloca(Type(type), Literal(size));
		}

		llvm::BranchInst* IREmitter::Branch(llvm::Value* pCondVal, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
		{
			assert(pCondVal != nullptr);
			assert(pThenBlock != nullptr);
			assert(pElseBlock != nullptr);

			return _builder.CreateCondBr(pCondVal, pThenBlock, pElseBlock);
		}

		llvm::BranchInst* IREmitter::Branch(llvm::BasicBlock* pDest)
		{
			assert(pDest != nullptr);

			return _builder.CreateBr(pDest);
		}

		llvm::StructType* IREmitter::Struct(const std::string& name, ValueTypeList& fields)
		{
			BindTypes(fields);
			return llvm::StructType::create(_context, _types, name);
		}

		llvm::Type* IREmitter::GetValueType(const ValueType type)
		{
			switch (type)
			{
			case ValueType::Void:
				return _builder.getVoidTy();
			case ValueType::Byte:
				return _builder.getInt8Ty();
			case ValueType::Short:
				return _builder.getInt16Ty();
			case ValueType::Int32:
				return _builder.getInt32Ty();
			case ValueType::Int64:
				return _builder.getInt64Ty();
			case ValueType::Double:
				return _builder.getDoubleTy();
			case ValueType::Char8:
				return _builder.getInt8Ty();
			default:
				throw new EmitterException(EmitterError::InvalidValueType);
			}
		}

		int IREmitter::SizeOf(const ValueType type)
		{
			switch (type)
			{
			case ValueType::Byte:
				return 8;
			case ValueType::Short:
				return 16;
			case ValueType::Int32:
				return 32;
			case ValueType::Int64:
				return 64;
			case ValueType::Double:
				return 8;
			case ValueType::Char8:
				return 8;
			default:
				throw new EmitterException(EmitterError::InvalidValueType);
			}
		}

		llvm::Constant* IREmitter::Integer(const ValueType type, const uint64_t value)
		{
			return llvm::ConstantInt::get(_context, llvm::APInt(SizeOf(type), value, true));
		}

		void IREmitter::BindTypes(const ValueTypeList& args)
		{
			_types.clear();
			for (auto arg = args.begin(); arg != args.end(); ++arg)
			{
				_types.push_back(Type(*arg));
			}
		}

		void IREmitter::BindArgTypes(const NamedValueTypeList& args)
		{
			_types.clear();
			for (auto arg = args.begin(); arg != args.end(); ++arg)
			{
				_types.push_back(Type(arg->second));
			}
		}

		void IREmitter::BindArgNames(llvm::Function* pfn, const NamedValueTypeList& args)
		{
			size_t i = 0;
			for (auto &arg : pfn->args())
			{
				arg.setName(args[i++].first);
			}
		}

		llvm::Function* IREmitter::CreateFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pTypeDef)
		{
			llvm::Function* pfn = llvm::Function::Create(pTypeDef, linkage, name, pModule);
			if (pfn == nullptr)
			{
				throw new EmitterException(EmitterError::InvalidFunction);
			}
			return pfn;
		}

		llvm::Value* IREmitter::Zero()
		{
			if (_pZeroLiteral == nullptr)
			{
				_pZeroLiteral = Literal(0);
			}
			return _pZeroLiteral;
		}
	}
}