////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRBlockRegion.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRBlockRegion.h"

// llvm
#include <llvm/IR/Function.h> // This include is necessary because of forward declarations in LLVM

namespace ell
{
namespace emitters
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

    void IRBlockRegion::SetStart(llvm::BasicBlock* pStart)
    {
        assert(pStart != nullptr);
        _pStart = pStart;
    }

    void IRBlockRegion::SetEnd(llvm::BasicBlock* pEnd)
    {
        assert(pEnd != nullptr);
        _pEnd = pEnd;
    }

    std::vector<llvm::BasicBlock*> IRBlockRegion::ToVector() const
    {
        std::vector<llvm::BasicBlock*> blocks;
        llvm::BasicBlock* pBlock = _pStart;
        while (true)
        {
            blocks.push_back(pBlock);
            if (pBlock == _pEnd)
            {
                break;
            }
            pBlock = pBlock->getNextNode();
        }

        return blocks;
    }

    //
    // IRBlockRegionList methods
    //
    IRBlockRegion* IRBlockRegionList::Add(llvm::BasicBlock* pBlock)
    {
        auto pRegion = std::make_shared<IRBlockRegion>(pBlock);
        _regions.push_back(pRegion);
        return pRegion.get();
    }

    IRBlockRegion* IRBlockRegionList::GetAt(size_t index)
    {
        return _regions[index].get();
    }

    void IRBlockRegionList::Clear()
    {
        _regions.clear();
    }
}
}
