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
    class SparseStlIterator : public IIndexValueIterator
    {
    public:
        // abbreviate iterator type, for improved readability
        using StlIteratorType = typename std::vector<ValueType>::const_iterator;

        /// Default copy ctor
        ///
        SparseStlIterator(const SparseStlIterator&) = default;

        /// Default move ctor
        ///
        SparseStlIterator(SparseStlIterator&&) = default;

        SparseStlIterator(const StlIteratorType& begin, const StlIteratorType& end);

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

    // #### TODO: put in tcc file
    template <typename ValueType>
    SparseStlIterator<ValueType> inline GetIterator(const std::vector<ValueType>& arr)
    {
        return SparseStlIterator<ValueType>(arr.cbegin(), arr.cend());
    }

    typedef std::vector<double> DoubleArray;
}

#include "../tcc/RealArray.tcc"
