#include "IRIfEmitter.h"
#include "IRFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		const std::string IfThenBlockName = "if.then";
		const std::string IfElseBlockName = "if.else";
		const std::string IfEndBlockName = "if.end";
		const std::string IfAfterBlockName = "if.after";

		IRIfEmitter::IRIfEmitter(IRFunctionEmitter& fn, llvm::BasicBlock* pPrevBlock)
			: _fn(fn), _pEndBlock(pPrevBlock)
		{
			_pAfterBlock = _fn.BlockAfter(ParentBlock(), IfAfterBlockName);
		}

		llvm::BasicBlock* IRIfEmitter::If(ComparisonType comparison, llvm::Value* pValue, llvm::Value* pTestValue)
		{
			EndPrev();
			CreateBlocks();
			_fn.CurrentBlock(_pCondBlock);
			_fn.Branch(comparison, pValue, pTestValue, _pThenBlock, _pEndBlock);
			_fn.CurrentBlock(_pThenBlock);
			return _pThenBlock;
		}

		llvm::BasicBlock* IRIfEmitter::Else()
		{
			assert(_pEndBlock != nullptr);  // Else was called twice, OR without a preceding If

			EndPrev();
			_pThenBlock = _pEndBlock;
			_pEndBlock = nullptr;
			_fn.CurrentBlock(_pThenBlock);
			return _pThenBlock;
		}

		void IRIfEmitter::End()
		{
			EndPrev();
			_fn.CurrentBlock(_pAfterBlock);
		}

		void IRIfEmitter::EndPrev()
		{
			if (_pThenBlock != nullptr)
			{
				_fn.CurrentBlock(_pThenBlock);
				_fn.Branch(_pAfterBlock);
			}
		}

		void IRIfEmitter::CreateBlocks()
		{
			_pCondBlock = ParentBlock();
			_pThenBlock = _fn.BlockAfter(_pCondBlock, IfThenBlockName);
			_pEndBlock = _fn.BlockAfter(_pThenBlock, IfEndBlockName);
		}

		llvm::BasicBlock* IRIfEmitter::ParentBlock()
		{
			return (_pEndBlock != nullptr) ?  _pEndBlock : _fn.CurrentBlock();
		}
	}
}