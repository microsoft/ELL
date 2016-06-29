#pragma once

#include "IREmitter.h"

namespace emll
{
	namespace compiler
	{
		class IRFunctionEmitter;

		class IRIfEmitter
		{
		public:
			IRIfEmitter(IRFunctionEmitter& fn, llvm::BasicBlock* pPrevBlock = nullptr);

			llvm::BasicBlock* If(ComparisonType comparison, llvm::Value* pValue, llvm::Value* pTestValue);
			llvm::BasicBlock* Else();
			void End();

		private:
			void CreateBlocks();
			void EndPrev();

			llvm::BasicBlock* ParentBlock();

		private:
			IRFunctionEmitter& _fn;
			llvm::BasicBlock* _pCondBlock = nullptr;
			llvm::BasicBlock* _pThenBlock = nullptr;
			llvm::BasicBlock* _pEndBlock = nullptr;
			llvm::BasicBlock* _pAfterBlock = nullptr;
			std::vector<llvm::BasicBlock*> _blocks;
		};
	}
}