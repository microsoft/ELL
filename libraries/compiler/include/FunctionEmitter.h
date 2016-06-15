#pragma once

#include "ValueType.h"
#include "LLVMEmitter.h"


namespace emll
{
	namespace compiler
	{
		class FunctionEmitter
		{
		public:
			FunctionEmitter(LLVMEmitter* pEmitter, llvm::Function* pfn);
			FunctionEmitter(const FunctionEmitter& src);

			llvm::Value* literal(int value)
			{
				return _pEmitter->literal(value);
			}
			llvm::Value* literal(int64_t value)
			{
				return _pEmitter->literal(value);
			}
			llvm::Value* literal(double value)
			{
				return _pEmitter->literal(value);
			}
			llvm::Value* literal(const std::string& value)
			{
				return _pEmitter->literal(value);
			}
			llvm::Value* cast(llvm::Value* pValue, ValueType destType)
			{
				return _pEmitter->addCast(pValue, destType);
			}
			llvm::BasicBlock* block(const std::string& label)
			{
				return _pEmitter->addBlock(_pfn, label);
			}
			llvm::BasicBlock* currentBlock()
			{
				return _pEmitter->currentBlock();
			}
			void currentBlock(llvm::BasicBlock* pBlock)
			{
				_pEmitter->setCurrentBlock(pBlock);
			}

			llvm::Value* call(const std::string& name, llvm::Value* pArg = nullptr);
			llvm::Value* call(const std::string& name, ValueList& args);
			llvm::Value* call(const std::string& name, std::initializer_list<llvm::Value*> args);
			void ret()
			{
				_pEmitter->addReturnVoid();
			}
			llvm::Value* ret(llvm::Value* value)
			{
				return _pEmitter->addReturn(value);
			}

			llvm::Value* op(OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal)
			{
				return _pEmitter->addBinaryOp(type, pLVal, pRVal);
			}
			llvm::Value* op(OperatorType type, llvm::iterator_range<llvm::Function::arg_iterator>& args);

			llvm::Function* pfn()
			{
				return _pfn;
			}
			llvm::iterator_range<llvm::Function::arg_iterator> args()
			{
				return _pfn->args();
			}
			llvm::Argument& firstArg()
			{
				return *(args().begin());
			}

			llvm::Value* var(ValueType type)
			{
				return _pEmitter->addVariable(type);
			}
			llvm::Value* var(ValueType type, int count)
			{
				return _pEmitter->addStackAlloc(type, count);
			}

			llvm::Value* load(llvm::Value* pPtr)
			{
				return _pEmitter->addLoad(pPtr);
			}
			llvm::Value* load(llvm::Value* pPtr, llvm::Value* pValue)
			{
				return _pEmitter->addLoad(pPtr, pValue);
			}
			llvm::Value* store(llvm::Value* pPtr, llvm::Value* pValue);

			llvm::Value* ptrOffsetA(llvm::Value* pPtr, int offset);
			llvm::Value* valueAtA(llvm::Value* pPtr, int offset);
			llvm::Value* setValueAtA(llvm::Value* pPtr, int offset, llvm::Value* pValue);

			llvm::Value* ptrOffsetH(llvm::Value* pPtr, int offset);
			llvm::Value* valueAtH(llvm::Value* pPtr, int offset);
			llvm::Value* setValueAtH(llvm::Value* pPtr, int offset, llvm::Value* pValue);

			llvm::Value* malloc(compiler::ValueType type, int64_t size);
			void free(llvm::Value* pValue);

			void verify()
			{
				llvm::verifyFunction(*_pfn);
			}

		private:
			llvm::Module* module()
			{
				return _pfn->getParent();
			}
			llvm::Function* resolveFunction(const std::string& name);

		private:
			llvm::Function* _pfn;
			LLVMEmitter* _pEmitter;
			ValueList _values;
		};

	}
}

