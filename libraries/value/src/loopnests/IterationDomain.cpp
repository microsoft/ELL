////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IterationDomain.cpp (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/IterationDomain.h"

namespace ell
{
namespace value
{
    namespace loopnests
    {

        IterationDomain::IterationDomain(const std::vector<IndexRange>& dimensions) :
            _dimensions(dimensions)
        {
            for (int d = 0; d < NumDimensions(); ++d)
            {
                _indexToDimensionMap[_dimensions[d].GetIndex().GetId()] = d;
            }
            //Assert(IsUnique(Transform(dimensions, [](auto x) { return x.GetIndex().GetName(); })), "Dimensions must have unique indices");
        }

        IterationDomain::IterationDomain(const std::initializer_list<IndexRange>& dimensions) :
            IterationDomain(std::vector<IndexRange>{ dimensions.begin(), dimensions.end() })
        {}

        int IterationDomain::NumDimensions() const
        {
            return static_cast<int>(_dimensions.size());
        }

        IndexRange IterationDomain::GetDimensionRange(int dimension) const
        {
            return _dimensions[dimension];
        }

        IndexRange IterationDomain::GetDimensionRange(const Index& index) const
        {
            return _dimensions[GetDimensionRangeFromIndex(index)];
        }

        const std::vector<IndexRange>& IterationDomain::GetRanges() const
        {
            return _dimensions;
        }

        int IterationDomain::GetDimensionRangeFromIndex(const Index& index) const
        {
            auto it = _indexToDimensionMap.find(index.GetId());
            return it->second;
        }

    } // namespace loopnests
} // namespace value
} // namespace ell
