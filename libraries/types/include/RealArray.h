// RealArray.h

#pragma once

#include "types.h"

// stl
#include <vector>

namespace types
{
    /// A read-only forward index-value iterator for an STL container
    ///

    // TODO: rename to something more correct
    template <typename ValueType>
    class StlIndexValueIterator : public IIndexValueIterator
    {
    public:
        /// abbreviation of std::vector iterator type, for improved readability
        using StlIteratorType = typename std::vector<ValueType>::const_iterator;

        /// Constructor
        ///
        /// <param name="begin"> STL iterator pointing at beginning of range to iterate over </param>
        /// <param name="end"> STL iterator pointing at end of range to iterate over </param>
        StlIndexValueIterator(const StlIteratorType& begin, const StlIteratorType& end);

        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        bool IsValid() const;

        /// Proceeds to the Next iterate
        ///
        void Next();

        /// \returns The current index-value pair
        ///
        IndexValue Get() const;

    protected:

        StlIteratorType _begin;
        StlIteratorType _end;
        uint64 _index = 0;
        void SkipZeros();
    };

    
    template <typename ValueType>
    StlIndexValueIterator<ValueType> GetIndexValueIterator(const std::vector<ValueType>& arr);

    // nice name for vector<double>
    typedef std::vector<double> DoubleArray;
}


#include "../tcc/RealArray.tcc"
