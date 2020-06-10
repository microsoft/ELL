////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IndexRange.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Index.h"
#include "Range.h"

#include <string>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        /// <summary>
        /// A range of integer values, used to express the interval that an index variable may take on.
        /// </summary>
        class IndexRange
        {
        public:
            IndexRange(const Index& index, const Range& range);
            IndexRange(const std::string& name, const Range& range);

            const Index& GetIndex() const;
            const std::string& GetName() const;
            int Begin() const;
            int End() const;
            int Size() const;
            int Increment() const;
            Range GetRange() const;

        private:
            Index _index;
            Range _range;

            friend inline bool operator==(const IndexRange& i1, const IndexRange& i2) { return (i1.GetIndex() == i2.GetIndex()) && (i1.GetRange() == i2.GetRange()); }
            friend inline bool operator!=(const IndexRange& i1, const IndexRange& i2) { return (i1.GetIndex() != i2.GetIndex()) || (i1.GetRange() != i2.GetRange()); }
            friend inline bool operator<(const IndexRange& i1, const IndexRange& i2) { return (i1.GetIndex() != i2.GetIndex()) ? (i1.GetIndex() == i2.GetIndex()) : (i1.GetRange() < i2.GetRange()); }
        };

    } // namespace loopnests
} // namespace value
} // namespace ell