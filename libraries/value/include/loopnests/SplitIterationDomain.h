////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SplitIterationDomain.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IterationDomain.h"
#include "SplitIndexRange.h"

#include <ostream>
#include <unordered_map>
#include <vector>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        /// <summary>
        /// An `IterationDomain` where some of the dimensions may have been split (tiled).
        /// </summary>
        class SplitIterationDomain
        {
        public:
            SplitIterationDomain(const IterationDomain& domain);

            int NumDimensions() const;
            int GetDimensionSize(const Index& dimensionIndex) const;
            int GetDimensionBegin(const Index& dimensionIndex) const;

            Range GetIndexRange(const Index& index) const;

            std::vector<Index> GetAllLoopIndices() const;

            const std::vector<Index>& GetIndicesForDimension(const Index& dimensionIndex) const;
            std::vector<Index> GetLoopIndicesForDimension(const Index& dimensionIndex) const;
            std::vector<Index> GetComputedIndicesForDimension(const Index& dimensionIndex) const;

            std::vector<Index> GetDependentIndices(const Index& index, bool includeSelf = false) const;
            std::vector<Index> GetDependentLoopIndices(const Index& index, bool includeSelf = false) const;

            bool Contains(const Index& index) const; // returns `true` if the given index belongs to this domain
            bool IsLoopIndex(const Index& index) const; // a leaf node in the index tree
            bool IsComputedIndex(const Index& index) const; // an interior node in the index tree
            bool IsDimension(const Index& index) const; // the index corresponding to the original range

            bool SameDimension(const Index& index1, const Index& index2) const;
            bool IsParentOf(const Index& parent, const Index& child) const; // is 'parent' the (immediate) parent of 'child'?
            bool IsChildOf(const Index& child, const Index& parent) const; // is 'child' a (immediate) child of 'parent'?
            bool DependsOn(const Index& index1, const Index& index2) const; // does 'index1' depend on 'index2'? (after a split, the parent depends on the new (leaf) indices)

            bool HasParentIndex(const Index& parent) const;

            /// <summary> Get the index that was split in order to create the given index </summary>
            Index GetParentIndex(const Index& parent) const;

            bool IsOuterSplitIndex(const Index& index) const;
            bool IsInnerSplitIndex(const Index& index) const;
            Index GetOuterSplitIndex(const Index& parent) const;
            Index GetInnerSplitIndex(const Index& parent) const;

            std::vector<Index> GetAllParentIndices(const Index& index) const;
            std::vector<Index> GetChildIndices(const Index& index) const;

            const SplitIndexRange& GetDimensionRange(const Index& index) const;
            SplitIndexRange& GetDimensionRange(const Index& index);

            const SplitIndexRange& GetDimensionRange(int offset) const;
            SplitIndexRange& GetDimensionRange(int offset);

            int NumSplits(const Index& dimensionIndex) const;
            Index GetBaseIndex(const Index& index) const;
            Index GetBaseIndex(int offset) const;
            bool IsPrimaryDimension(const Index& index) const;

            SplitIndex Split(const Index& index, int splitSize);

            void Print(std::ostream& os) const;

        private:
            int GetOffsetFromIndex(const Index& index) const;

            std::unordered_map<Index, Index> _baseIndices;
            std::vector<SplitIndexRange> _dimensions;
            std::unordered_map<Index, int> _indexToOffsetMap;
        };

    } // namespace loopnests
} // namespace value
} // namespace ell
