////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRBlockRegion.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// llvm
#include <llvm/IR/BasicBlock.h>

// stl
#include <memory>
#include <vector>

namespace ell
{
namespace emitters
{
    ///<summary> A span of code can be compiled into one or more CONTIGUOUS blocks. This trival class helps track them. </summary>
    class IRBlockRegion
    {
    public:
        /// <summary> Construct a single-block region from a pointer to a block. </summary>
        ///
        /// <param name="pStart"> A pointer to the first llvm block. </param>
        IRBlockRegion(llvm::BasicBlock* pStart);

        /// <summary> Gets a pointer to the first block in the region. </summary>
        ///
        /// <returns> A pointer to the first llvm block in the region. </returns>
        llvm::BasicBlock* Start() const { return _pStart; }

        /// <summary> Gets a pointer to the end of the block region. </summary>
        ///
        /// <returns> A pointer to the end of the llvm block region. </returns>
        llvm::BasicBlock* End() const { return _pEnd; }

        /// <summary> Set the start pointer. </summary>
        ///
        /// <param name="pStart"> Pointer to the start of an llvm block region. </param>
        void SetStart(llvm::BasicBlock* pStart);

        /// <summary> Sets the end pointer. </summary>
        ///
        /// <param name="pEnd"> Pointer to the end of an llvm block region. </param>
        void SetEnd(llvm::BasicBlock* pEnd);

        /// <summary> True if the region has exactly 1 block. </summary>
        ///
        /// <returns> true if single block, false if not. </returns>
        bool IsSingleBlock() const;

        /// <summary>
        /// Is this a top level block region or logically "nested/scoped" below another one.
        /// </summary>
        ///
        /// <returns> A reference to a bool that indicates if this block region is top level. </returns>
        bool& IsTopLevel() { return _isTopLevel; }

        /// <summary> Returns a std::vector of block pointers that represents this region. </summary>
        ///
        /// <returns> A std::vector of block pointers. </returns>
        std::vector<llvm::BasicBlock*> ToVector() const;

    private:
        llvm::BasicBlock* _pStart = nullptr;
        llvm::BasicBlock* _pEnd = nullptr;
        bool _isTopLevel = true;
    };

    ///<summary>Maintains a list of block regions</summary>
    class IRBlockRegionList
    {
    public:
        /// <summary> Creates a new single-block region from a pointer to a block and adds it to the list. </summary>
        ///
        /// <param name="pStart"> Pointer to an llvm block. </param>
        ///
        /// <returns> Pointer to the new block region. </returns>
        IRBlockRegion* Add(llvm::BasicBlock* pStart);

        /// <summary> Gets the number of regions in this list. </summary>
        ///
        /// <returns> The number of regions in this list. </returns>
        size_t Size() const { return _regions.size(); }

        /// <summary> Returns the region at a given index. </summary>
        ///
        /// <param name="index"> Zero-based index of the region. </param>
        ///
        /// <returns> Pointer to a block region. </returns>
        IRBlockRegion* GetAt(size_t index);

        /// <summary> Erase all the regions in the list. </summary>
        void Clear();

    private:
        std::vector<std::shared_ptr<IRBlockRegion>> _regions;
    };
}
}
