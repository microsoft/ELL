#pragma once

#include "ValueType.h"
#include "LLVMEmitter.h"


namespace emll
{
	namespace compiler
	{
		namespace ir
		{
			class FunctionEmitter
			{
			public:
				FunctionEmitter(void) {}
				FunctionEmitter(LLVMEmitter* pEmitter, llvm::Function* pfn);
				FunctionEmitter(const FunctionEmitter& src);

				llvm::Value* Literal(int value)
				{
					return _pEmitter->Literal(value);
				}
				llvm::Value* Literal(int64_t value)
				{
					return _pEmitter->Literal(value);
				}
				llvm::Value* Literal(double value)
				{
					return _pEmitter->Literal(value);
				}
				llvm::Value* Literal(const std::string& value)
				{
					return _pEmitter->Literal(value);
				}
				llvm::Value* Arg(llvm::ilist_iterator<llvm::Argument>& arg)
				{
					return _pEmitter->Load(&(*arg));
				}
				llvm::Value* Cast(llvm::Value* pValue, ValueType destType)
				{
					return _pEmitter->Cast(pValue, destType);
				}
				llvm::BasicBlock* Block(const std::string& label)
				{
					return _pEmitter->Block(_pfn, label);
				}
				llvm::BasicBlock* CurrentBlock()
				{
					return _pEmitter->CurrentBlock();
				}
				void CurrentBlock(llvm::BasicBlock* pBlock)
				{
					_pEmitter->SetCurrentBlock(pBlock);
				}

				llvm::Value* Call(const std::string& name, llvm::Value* pArg = nullptr);
				llvm::Value* Call(const std::string& name, ValueList& args);
				llvm::Value* Call(const std::string& name, std::initializer_list<llvm::Value*> args);
				void Ret()
				{
					_pEmitter->ReturnVoid();
				}
				llvm::Value* Ret(llvm::Value* value)
				{
					return _pEmitter->Return(value);
				}

				llvm::Value* Op(OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal)
				{
					return _pEmitter->BinaryOp(type, pLVal, pRVal);
				}
				llvm::Value* Op(OperatorType type, llvm::iterator_range<llvm::Function::arg_iterator>& args);

				llvm::iterator_range<llvm::Function::arg_iterator> Args()
				{
					return _pfn->args();
				}
				llvm::Argument& FirstArg()
				{
					return *(Args().begin());
				}
				llvm::Value* Var(ValueType type)
				{
					return _pEmitter->Variable(type);
				}
				llvm::Value* Var(ValueType type, const std::string& name)
				{
					return _pEmitter->Variable(type, name);
				}

				llvm::Value* Var(ValueType type, int count)
				{
					return _pEmitter->StackAlloc(type, count);
				}

				llvm::Value* Load(llvm::Value* pPtr)
				{
					return _pEmitter->Load(pPtr);
				}
				llvm::Value* Load(llvm::Value* pPtr, llvm::Value* pValue)
				{
					return _pEmitter->Load(pPtr, pValue);
				}
				llvm::Value* Store(llvm::Value* pPtr, llvm::Value* pValue);

				llvm::Value* PtrOffsetA(llvm::Value* pPtr, int offset);
				llvm::Value* ValueAtA(llvm::Value* pPtr, int offset);
				llvm::Value* SetValueAtA(llvm::Value* pPtr, int offset, llvm::Value* pValue);

				llvm::Value* PtrOffsetH(llvm::Value* pPtr, int offset);
				llvm::Value* ValueAtH(llvm::Value* pPtr, int offset);
				llvm::Value* SetValueAtH(llvm::Value* pPtr, int offset, llvm::Value* pValue);

				llvm::Value* Malloc(ValueType type, int64_t size);
				void Free(llvm::Value* pValue);

				void Verify()
				{
					llvm::verifyFunction(*_pfn);
				}

			private:
				llvm::Module* Module()
				{
					return _pfn->getParent();
				}
				llvm::Function* ResolveFunction(const std::string& name);

			private:
				llvm::Function* _pfn;
				LLVMEmitter* _pEmitter;
				ValueList _values;
			};
		}
	}
}

