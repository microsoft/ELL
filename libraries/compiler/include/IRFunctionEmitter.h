////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRFunctionEmitter.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IREmitter.h"
#include "IRLoopEmitter.h"
#include "IRIfEmitter.h"

namespace emll
{
	namespace compiler
	{
		///<summary>Used to emit code INTO an existing LLVM IR Function</summary>
		class IRFunctionEmitter
		{
		public:
			///<summary>Create a new empty function emitter.</summary>
			IRFunctionEmitter(void) {}
			///<summary>Copy constructor for the emitter</summary>
			IRFunctionEmitter(const IRFunctionEmitter& src);
			///<summary>Create a new emitter to emit code INTO the given function, using the given IR emitter</summary>
			IRFunctionEmitter(IREmitter* pEmitter, llvm::Function* pfn);
			
			///<summary>Initialize the emitter</summary>
			void Init(IREmitter* pEmitter, llvm::Function* pfn);

			///<summary>Emit a literal into the function</summary>
			template<typename T>
			llvm::Value* Literal(T value);

			///<summary>Emit a load for the function argument</summary>
			llvm::Value* LoadArg(llvm::ilist_iterator<llvm::Argument>& arg)
			{
				return _pEmitter->Load(&(*arg));
			}

			///<summary>Emit a typecast to the given ValueType</summary>
			llvm::Value* Cast(llvm::Value* pValue, ValueType destType)
			{
				return _pEmitter->Cast(pValue, destType);
			}
			///<summary>Explict typecast from float to integer</summary>
			llvm::Value* CastFloatToInt(llvm::Value* pValue)
			{
				return _pEmitter->CastFloat(pValue, ValueType::Int32);
			}
			///<summary>Explict downcast from integer to byte</summary>
			llvm::Value* CastBoolToInt(llvm::Value* pValue)
			{
				return _pEmitter->CastInt(pValue, ValueType::Int32);
			}

			///<summary>Emit a typecast to template type</summary>
			template<typename T>
			llvm::Value* Cast(llvm::Value* pValue);
			
				
			///<summary>Emit a call to a function with a single optional argument</summary>
			llvm::Value* Call(const std::string& name, llvm::Value* pArg = nullptr);
			///<summary>Emit a call to a function with arguments</summary>
			llvm::Value* Call(const std::string& name, IRValueList& args);
			///<summary>Emit a call to a function with arguments</summary>
			llvm::Value* Call(const std::string& name, std::initializer_list<llvm::Value*> args);
			///<summary>Emit a call to a function with arguments</summary>
			llvm::Value* Call(llvm::Function* pfn, std::initializer_list<llvm::Value*> args);

			///<summary>Emit a return - the function returns void</summary>
			void Ret()
			{
				_pEmitter->ReturnVoid();
			}
			///<summary>Emit a return - the function returns the given value</summary>
			llvm::Value* Ret(llvm::Value* value)
			{
				return _pEmitter->Return(value);
			}
			///<summary>Emit a binary operator with 2 scalar args</summary>
			llvm::Value* Op(OperatorType type, llvm::Value* pLVal, llvm::Value* pRVal)
			{
				return _pEmitter->BinaryOp(type, pLVal, pRVal);
			}
			///<summary>Emit a binary operator with 2 scalar args</summary>
			llvm::Value* Op(OperatorType type, llvm::iterator_range<llvm::Function::arg_iterator>& args);
			
			///<summary>Emit binary operator over 2 equal sized vector args - the operator is applied to each pair of scalars. Also supply an aggregator function</summary>
			void OpV(OperatorType type, size_t count, llvm::Value* pLVal, llvm::Value* pRVal, std::function<void(llvm::Value*, llvm::Value*)> aggregator);
			///<summary>Emit binary operator over 2 equal sized vector args - the operator is applied to each pair of scalars. Also supply an aggregator function</summary>
			void OpV(OperatorType type, llvm::Value* pCount, llvm::Value* pLVal, llvm::Value* pRVal, std::function<void(llvm::Value*, llvm::Value*)> aggregator);
			///<summary>Emit binary operator over 2 equal sized vector args - the operator is applied to each pair of scalars. Also supply an aggregator function</summary>
			void OpV(OperatorType type, size_t count, llvm::Value* pLVal, int startAtL, llvm::Value* pRVal, int startAtR, std::function<void(llvm::Value*, llvm::Value*)> aggregator);

			///<summary>Emit a branch to the given block</summary>
			void Branch(llvm::BasicBlock* pDestBlock)
			{
				_pEmitter->Branch(pDestBlock);
			}
			///<summary>Emit a conditional branch</summary>
			void Branch(llvm::Value* pCondVal, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock)
			{
				_pEmitter->Branch(pCondVal, pThenBlock, pElseBlock);
			}
			///<summary>Emit a conditional branch</summary>
			void Branch(ComparisonType comparision, llvm::Value* pValue, llvm::Value* pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);
			///<summary>Emit a comparison</summary>
			llvm::Value* Cmp(ComparisonType type, llvm::Value* pValue, llvm::Value* pTestValue)
			{
				return _pEmitter->Cmp(type, pValue, pTestValue);
			}

			//-----------------------------------
			//
			// Block management
			//
			//-----------------------------------

			///<summary>The current code block that code is being emitted into</summary>
			llvm::BasicBlock* CurrentBlock()
			{
				return _pEmitter->CurrentBlock();
			}
			/// <summary>Set the block that subsequent code will go into.</summary> 
			/// <returns>The previous block</returns>
			llvm::BasicBlock* CurrentBlock(llvm::BasicBlock* pBlock);

			///<summary>Emit a new labelled code block. The "current" block does not change</summary>
			llvm::BasicBlock* Block(const std::string& label)
			{
				return _pEmitter->Block(_pfn, label);
			}
			///<summary>Emit a new labelled code block. The block is inserted after the given previous block</summary>
			llvm::BasicBlock* BlockAfter(llvm::BasicBlock* pPrevBlock, const std::string& label)
			{
				return _pEmitter->BlockAfter(_pfn, pPrevBlock, label);
			}
			///<summary>The given block is added to the end of the function's block list</summary>
			void AppendBlock(llvm::BasicBlock* pBlock);

			//------------------------------------------
			//
			// Variables
			//
			//------------------------------------------

			///<summary>Iterate over this function's arguments</summary>
			llvm::iterator_range<llvm::Function::arg_iterator> Args()
			{
				return _pfn->args();
			}
			///<summary>Get the first function argument</summary>
			llvm::Argument& FirstArg()
			{
				return *(Args().begin());
			}
			///<summary>Emit a stack scalar</summary>
			llvm::Value* Var(ValueType type)
			{
				return _pEmitter->Variable(type);
			}
			///<summary>Emit a named stack scalar </summary>
			llvm::Value* Var(ValueType type, const std::string& name)
			{
				return _pEmitter->Variable(type, name);
			}
			///<summary>Emit a stack vector of the given size</summary>
			llvm::Value* Var(ValueType type, int count)
			{
				return _pEmitter->StackAlloc(type, count);
			}

			//------------------------------------------
			//
			// Load and tore
			//
			//------------------------------------------
			llvm::Value* Load(llvm::Value* pPtr)
			{
				return _pEmitter->Load(pPtr);
			}
			llvm::Value* Load(llvm::Value* pPtr, const std::string& name)
			{
				return _pEmitter->Load(pPtr, name);
			}

			llvm::Value* Store(llvm::Value* pPtr, llvm::Value* pValue);
			llvm::Value* OpAndUpdate(llvm::Value* pPtr, OperatorType op, llvm::Value* pValue);

			llvm::Value* PtrOffset(llvm::Value* pPtr, llvm::Value* pOffset);
			llvm::Value* PtrOffset(llvm::Value* pPtr, int offset);
			llvm::Value* ValueAt(llvm::Value* pPtr, llvm::Value* pOffset);
			llvm::Value* ValueAt(llvm::Value* pPtr, int offset);
			llvm::Value* SetValueAt(llvm::Value* pPtr, llvm::Value* pOffset, llvm::Value* pVal);
			llvm::Value* SetValueAt(llvm::Value* pPtr, int offset, llvm::Value* pVal);

			llvm::Value* PtrOffsetA(llvm::Value* pPtr, int offset);
			llvm::Value* PtrOffsetA(llvm::Value* pPtr, llvm::Value* pOffset, const std::string& name = "");

			llvm::Value* ValueAtA(llvm::Value* pPtr, int offset);
			llvm::Value* ValueAtA(llvm::Value* pPtr, llvm::Value* pOffset);
			llvm::Value* SetValueAtA(llvm::Value* pPtr, int offset, llvm::Value* pValue);
			llvm::Value* SetValueAtA(llvm::Value* pPtr, llvm::Value* pOffset, llvm::Value* pValue);

			llvm::Value* PtrOffsetH(llvm::Value* pPtr, int offset);
			llvm::Value* PtrOffsetH(llvm::Value* pPtr, llvm::Value* pOffset);
			llvm::Value* ValueAtH(llvm::Value* pPtr, int offset);
			llvm::Value* ValueAtH(llvm::Value* pPtr, llvm::Value* pOffset);
			llvm::Value* SetValueAtH(llvm::Value* pPtr, int offset, llvm::Value* pValue);

			llvm::Value* Ptr(llvm::GlobalVariable* pGlobal);
			llvm::Value* PtrOffset(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset);
			llvm::Value* PtrOffset(llvm::GlobalVariable* pGlobal, llvm::Value* , llvm::Value* pFieldOffset);

			llvm::Value* ValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset);
			llvm::Value* SetValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pVal);

			//------------------------------------------
			//
			// Expressions
			//
			//------------------------------------------
			IRForLoopEmitter ForLoop();
			IRIfEmitter If();

			//------------------------------------------
			//
			// Standard useful function calls
			//
			//------------------------------------------
			llvm::Value* Malloc(ValueType type, int64_t size);
			template<typename T>
			llvm::Value* Malloc(int64_t count);

			void Free(llvm::Value* pValue);

			llvm::Value* Print(const std::string& text);
			llvm::Value* Printf(std::initializer_list<llvm::Value*> args);

			template<typename T>
			void MemMove(llvm::Value* pPtr, int fromOffset, int destOffset, int count);
			template<typename T>
			void MemCopy(llvm::Value* pPtrSrc, int srcOffset, llvm::Value* pPtrDest, int destOffset, int count);

			//------------------------------------------
			//
			// Inline common code generators
			//
			//------------------------------------------
			///<summary>Emit IR to printf each item in the given vector</summary>
			void PrintForEach(const std::string& formatString, llvm::Value* pVector, int count);

			///<summary>Emit IR to compute a DOT product</summary>
			llvm::Value* DotProductF(int count, llvm::Value* pLVal, llvm::Value* pRVal);
			///<summary>Emit IR to compute a DOT product</summary>
			void DotProductF(int count, llvm::Value* pLVal, llvm::Value* pRVal, llvm::Value* pDest);
			///<summary>Emit IR to compute a DOT product</summary>
			void DotProductF(llvm::Value* pCount, llvm::Value* pLVal, llvm::Value* pRVal, llvm::Value* pDest);

			///<summary>Implements the mechanics of a shift register</summary>
			template<typename T>
			void ShiftAndUpdate(llvm::Value* pBuffer, int bufferCount, int shiftCount, llvm::Value* pNewData, llvm::Value* pShiftedData = nullptr);

			void Verify()
			{
				llvm::verifyFunction(*_pfn);
			}

			llvm::Function* Function()
			{
				return _pfn;
			}

		private:
			llvm::Module* Module()
			{
				return _pfn->getParent();
			}
			llvm::Function* ResolveFunction(const std::string& name);

		private:
			llvm::Function* _pfn = nullptr;
			IREmitter* _pEmitter = nullptr;
			IRValueList _values;
		};
	}
}

#include "../tcc/IRFunctionEmitter.tcc"

