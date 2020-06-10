////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SplitIterationDomain.cpp (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/SplitIterationDomain.h"

namespace ell
{
namespace value
{
    namespace loopnests
    {
        SplitIterationDomain::SplitIterationDomain(const IterationDomain& domain)
        {
            int numDimensions = domain.NumDimensions();
            for (int d = 0; d < numDimensions; ++d)
            {
                _dimensions.emplace_back(domain.GetDimensionRange(d));
                _indexToOffsetMap[_dimensions.back().GetDimensionIndex()] = d;
                for (auto i : _dimensions.back().GetIndices())
                {
                    _baseIndices.emplace(i, _dimensions.back().GetDimensionIndex());
                }
            }
            //Assert(IsUnique(Transform(domain.GetRanges(), [](auto x) { return x.GetDimensionIndex().GetName(); })), "Dimensions must have unique indices");
        }

        int SplitIterationDomain::NumDimensions() const
        {
            return static_cast<int>(_dimensions.size());
        }

        int SplitIterationDomain::GetDimensionSize(const Index& dimensionIndex) const
        {
            return GetDimensionRange(dimensionIndex).GetSize();
        }

        int SplitIterationDomain::GetDimensionBegin(const Index& dimensionIndex) const
        {
            return GetDimensionRange(dimensionIndex).GetBegin();
        }

        Range SplitIterationDomain::GetIndexRange(const Index& index) const
        {
            return GetDimensionRange(index).GetIndexRange(index);
        }

        const std::vector<Index>& SplitIterationDomain::GetIndicesForDimension(const Index& dimensionIndex) const
        {
            return GetDimensionRange(dimensionIndex).GetIndices();
        }

        std::vector<Index> SplitIterationDomain::GetAllLoopIndices() const
        {
            std::vector<Index> result;
            for (int i = 0; i < NumDimensions(); ++i)
            {
                auto dimensionIndices = GetDimensionRange(i).GetLoopIndices();
                result.insert(result.end(), dimensionIndices.begin(), dimensionIndices.end());
            }
            return result;
        }

        std::vector<Index> SplitIterationDomain::GetLoopIndicesForDimension(const Index& dimensionIndex) const
        {
            return GetDimensionRange(dimensionIndex).GetLoopIndices();
        }

        std::vector<Index> SplitIterationDomain::GetComputedIndicesForDimension(const Index& dimensionIndex) const
        {
            return GetDimensionRange(dimensionIndex).GetComputedIndices();
        }

        std::vector<Index> SplitIterationDomain::GetDependentIndices(const Index& index, bool includeSelf) const
        {
            return GetDimensionRange(index).GetDependentIndices(index, includeSelf);
        }

        std::vector<Index> SplitIterationDomain::GetDependentLoopIndices(const Index& index, bool includeSelf) const
        {
            return GetDimensionRange(index).GetDependentLoopIndices(index, includeSelf);
        }

        bool SplitIterationDomain::Contains(const Index& index) const
        {
            return GetDimensionRange(index).Contains(index);
        }

        bool SplitIterationDomain::IsLoopIndex(const Index& index) const
        {
            return GetDimensionRange(index).IsLoopIndex(index);
        }

        bool SplitIterationDomain::IsComputedIndex(const Index& index) const
        {
            return GetDimensionRange(index).IsComputedIndex(index);
        }

        bool SplitIterationDomain::IsDimension(const Index& index) const
        {
            return GetBaseIndex(index) == index;
        }

        bool SplitIterationDomain::SameDimension(const Index& index1, const Index& index2) const
        {
            return GetBaseIndex(index1) == GetBaseIndex(index2);
        }

        bool SplitIterationDomain::IsParentOf(const Index& parent, const Index& child) const
        {
            if (!SameDimension(parent, child))
            {
                return false;
            }
            return GetDimensionRange(parent).IsParentOf(parent, child);
        }

        bool SplitIterationDomain::IsChildOf(const Index& child, const Index& parent) const
        {
            if (!SameDimension(child, parent))
            {
                return false;
            }
            return GetDimensionRange(child).IsChildOf(child, parent);
        }

        bool SplitIterationDomain::DependsOn(const Index& index1, const Index& index2) const
        {
            if (!SameDimension(index1, index2))
            {
                return false;
            }
            return GetDimensionRange(index1).DependsOn(index1, index2);
        }

        bool SplitIterationDomain::HasParentIndex(const Index& parent) const
        {
            return GetDimensionRange(parent).HasParentIndex(parent);
        }

        Index SplitIterationDomain::GetParentIndex(const Index& parent) const
        {
            return GetDimensionRange(parent).GetParentIndex(parent);
        }

        bool SplitIterationDomain::IsOuterSplitIndex(const Index& index) const
        {
            return GetDimensionRange(index).IsOuterSplitIndex(index);
        }
        bool SplitIterationDomain::IsInnerSplitIndex(const Index& index) const
        {
            return GetDimensionRange(index).IsInnerSplitIndex(index);
        }
        Index SplitIterationDomain::GetOuterSplitIndex(const Index& parent) const
        {
            return GetDimensionRange(parent).GetOuterSplitIndex(parent);
        }
        Index SplitIterationDomain::GetInnerSplitIndex(const Index& parent) const
        {
            return GetDimensionRange(parent).GetInnerSplitIndex(parent);
        }

        std::vector<Index> SplitIterationDomain::GetAllParentIndices(const Index& index) const
        {
            return GetDimensionRange(index).GetAllParentIndices(index);
        }

        std::vector<Index> SplitIterationDomain::GetChildIndices(const Index& index) const
        {
            return GetDimensionRange(index).GetChildIndices(index);
        }

        const SplitIndexRange& SplitIterationDomain::GetDimensionRange(int offset) const
        {
            return _dimensions[offset];
        }

        SplitIndexRange& SplitIterationDomain::GetDimensionRange(int offset)
        {
            return _dimensions[offset];
        }

        const SplitIndexRange& SplitIterationDomain::GetDimensionRange(const Index& index) const
        {
            auto offset = GetOffsetFromIndex(index);
            return _dimensions[offset];
        }

        SplitIndexRange& SplitIterationDomain::GetDimensionRange(const Index& index)
        {
            auto offset = GetOffsetFromIndex(index);
            return _dimensions[offset];
        }

        int SplitIterationDomain::NumSplits(const Index& index) const
        {
            auto offset = GetOffsetFromIndex(index);
            return _dimensions[offset].NumSplits();
        }

        SplitIndex SplitIterationDomain::Split(const Index& index, int splitSize)
        {
            auto baseIndex = GetBaseIndex(index);
            auto offset = GetOffsetFromIndex(index);
            auto result = _dimensions[offset].Split(index, splitSize);
            _baseIndices.emplace(result.inner, baseIndex);
            _baseIndices.emplace(result.outer, baseIndex);
            return result;
        }

        bool SplitIterationDomain::IsPrimaryDimension(const Index& index) const
        {
            return _indexToOffsetMap.count(index) != 0;
        }

        int SplitIterationDomain::GetOffsetFromIndex(const Index& index) const
        {
            auto baseIndex = GetBaseIndex(index);
            return _indexToOffsetMap.at(baseIndex);
        }

        Index SplitIterationDomain::GetBaseIndex(const Index& index) const
        {
            auto mapIndex = _baseIndices.find(index);
            if (mapIndex != _baseIndices.end())
            {
                return mapIndex->second;
            }
            else
            {
                return index;
            }
        }

        Index SplitIterationDomain::GetBaseIndex(int offset) const
        {
            return _dimensions[offset].GetDimensionIndex();
        }

        void SplitIterationDomain::Print(std::ostream& os) const
        {
            for (const auto& d : _dimensions)
            {
                d.Print(os);
            }
        }

    } // namespace loopnests
} // namespace value
} // namespace ell
