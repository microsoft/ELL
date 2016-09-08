////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRBlockRegion.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRBlockRegion.h"
#include "IRFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		IRBlockRegion::IRBlockRegion(llvm::BasicBlock* pStart)
			: _pStart(pStart), _pEnd(pStart)
		{
			assert(pStart != nullptr);
		}

		bool IRBlockRegion::IsSingleBlock() const
		{
			return (_pStart == _pEnd);
		}

		void IRBlockRegion::SetEnd(llvm::BasicBlock* pEnd)
		{
			assert(pEnd != nullptr);
			_pEnd = pEnd;
		}

		IRBlockRegionList::IRBlockRegionList(IRFunctionEmitter& fn)
			: _fn(fn)
		{
		}

		IRBlockRegion* IRBlockRegionList::Add(llvm::BasicBlock* pBlock)
		{
			auto pRegion = std::make_shared<IRBlockRegion>(pBlock);
			_regions.push_back(pRegion);
			return pRegion.get();
		}

		void IRBlockRegionList::Link(IRBlockRegion* pTopRegion, IRBlockRegion* pBottomRegion)
		{
			assert(pTopRegion != nullptr && pBottomRegion != nullptr);

			if (pTopRegion->IsSingleBlock())
			{
				_fn.LinkBlocks(pTopRegion->Start(), pBottomRegion->Start());
			}
			else
			{
				_fn.LinkBlocks(pTopRegion->End(), pBottomRegion->Start());
			}
		}

		void IRBlockRegionList::Link()
		{
			llvm::BasicBlock* pCurBlock = _fn.CurrentBlock();
			IRBlockRegion* pPrevRegion = nullptr;
			for (size_t i = 0; i < _regions.size(); ++i)
			{
				IRBlockRegion* pRegion = _regions[i].get();
				if (pPrevRegion != nullptr)
				{
					_fn.CurrentBlock(pPrevRegion->End());
					_fn.Branch(pRegion->Start());
				}
				pPrevRegion = pRegion;
			}
			_fn.CurrentBlock(pCurBlock);
		}
	}
}