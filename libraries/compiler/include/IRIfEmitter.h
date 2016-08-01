#pragma once

#include "IREmitter.h"

namespace emll
{
	namespace compiler
	{
		class IRFunctionEmitter;

		///<summary>Class that helps with emitting If, Then,  Else blocks.</summary>
		class IRIfEmitter
		{
		public:
			///<summary>Constuct an emitter for the given function. If pPrevBlock was supplied, injects new blocks after it</summary>
			IRIfEmitter(IRFunctionEmitter& fn, llvm::BasicBlock* pPrevBlock = nullptr);
			///<summary>Emit a comparison of pValue to pTestValue and a branch to the "Then" block. Makes the Then block the current block</summary>
			///<returns>Returns the "Then" block</returns>
			llvm::BasicBlock* If(ComparisonType comparison, llvm::Value* pValue, llvm::Value* pTestValue);
			///<summary>Pure syntactic sugar for readability - works just liek "If"</summary>
			///<returns>Returns the "Then" block</returns>
			llvm::BasicBlock* IfElse(ComparisonType comparison, llvm::Value* pValue, llvm::Value* pTestValue)
			{
				return If(comparison, pValue, pTestValue);
			}
			///<summary>Emits an 'else' block, and makes it the current block</summary>
			///<returns>Returns the "Else block</returns>
			llvm::BasicBlock* Else();
			///<summary>Ends the if else block, injecting appropriate branches</summary>
			void End();

		private:
			void CreateBlocks();
			void EndPrev();

			llvm::BasicBlock* ParentBlock();

		private:
			IRFunctionEmitter& _fn;						// Function we are emitting into
			llvm::BasicBlock* _pCondBlock = nullptr;		// Block into which the conditional instructions are emitted -- the "if"
			llvm::BasicBlock* _pThenBlock = nullptr;		// Block into which the "Then" instructions are being emitted
			llvm::BasicBlock* _pEndBlock = nullptr;		// The end block. 
			llvm::BasicBlock* _pAfterBlock = nullptr;	// Block where code subsequent to the "if, else" will be emitted. The end block always branches here
			std::vector<llvm::BasicBlock*> _blocks;
		};
	}
}