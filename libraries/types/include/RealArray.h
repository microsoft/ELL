// RealArray.h

#pragma once

#include "types.h"

// stl
#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#include <type_traits>
using std::enable_if;
using std::enable_if_t;
using std::is_base_of;

namespace types
{
    /// A read-only forward iterator for the sparse binary vector.
    ///
    template <typename ValueType>
    class SparseStlIterator : public IIndexValueIterator
    {
    public:
        // abbreviate iterator type, for improved readability
        using StlIteratorType = typename vector<ValueType>::const_iterator;

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

        // members
        StlIteratorType _begin;
        StlIteratorType _end;
        uint64 _index = 0;
        void SkipZeros();
    };


    template <typename ValueType>
    SparseStlIterator<ValueType> inline GetIterator(const std::vector<ValueType>& arr)
    {
        return SparseStlIterator<ValueType>(arr.cbegin(), arr.cend());
    }


    template <typename ValueType>
    class RealArray : public vector<ValueType>
    {
    public:
       
        /// Ctor
        ///
        RealArray(uint64 size = 0);

        /// Default copy ctor
        ///
        RealArray(const RealArray<ValueType>&) = default;

        /// Default move ctor
        ///
        RealArray(RealArray<ValueType>&&) = default;

        ValueType* GetDataPointer();
        const ValueType* GetDataPointer() const;

        /// \returns An Iterator that points to the beginning of the array.
        ///
        using Iterator = SparseStlIterator<ValueType>;
        Iterator GetIterator() const;
    };


    typedef RealArray<double> DoubleArray;
    typedef RealArray<float> FloatArray;
}

#include "../tcc/RealArray.tcc"
