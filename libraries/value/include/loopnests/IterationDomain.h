////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IterationDomain.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexRange.h"

#include <initializer_list>
#include <map>
#include <vector>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        /// <summary>
        /// The set of all points (IterationVectors) to be visited by a loop or loop nest.
        /// </summary>
        class IterationDomain
        {
        public:
            IterationDomain() = default;
            IterationDomain(const std::vector<IndexRange>& ranges);
            IterationDomain(const std::initializer_list<IndexRange>& ranges);

            int NumDimensions() const;
            IndexRange GetDimensionRange(int dimension) const;
            IndexRange GetDimensionRange(const Index& index) const;
            const std::vector<IndexRange>& GetRanges() const;

        private:
            int GetDimensionRangeFromIndex(const Index& index) const;

            std::vector<IndexRange> _dimensions;
            std::map<Index::Id, int> _indexToDimensionMap;
        };

    } // namespace loopnests
} // namespace value
} // namespace ell
