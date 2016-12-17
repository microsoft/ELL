////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlIndexValueIterator.h (data)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexValue.h"

// stl
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

namespace ell
{
namespace data
{
    /// <summary> A read-only forward index-value iterator for an STL container </summary>
    template <typename IteratorType>
    class StlIndexValueIterator : public IIndexValueIterator
    {
    public:
        /// <summary> Constructs an instance of StlIndexValueIterator. </summary>
        ///
        /// <param name="begin"> STL iterator pointing at beginning of range to iterate over </param>
        /// <param name="end"> STL iterator pointing at end of range to iterate over </param>
        StlIndexValueIterator(const IteratorType& begin, const IteratorType& end);

        /// <summary> Returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const { return _begin != _end; }

        /// <summary> Proceeds to the Next iterate </summary>
        void Next();

        /// <summary> Returns The current index-value pair </summary>
        ///
        /// <returns> The current index-value pair </returns>
        IndexValue Get() const { return IndexValue{ _index, (double)*_begin }; }

    protected:
        IteratorType _begin;
        IteratorType _end;
        size_t _index = 0;
        void SkipZeros();
    };

    /// <summary> Handy type alias for a StlIndexValueIterator over a std::vector </summary>
    template <typename ValueType>
    using VectorIndexValueIterator = StlIndexValueIterator<typename std::vector<ValueType>::const_iterator>;

    /// <summary> Creates an StlIndexValueIteratorAdapter for a std::vector </summary>
    ///
    /// <param name="container"> A std::vector to iterate over </param>
    /// <returns> An StlIndexValueIteratorAdapter that iterates over the given vector </returns>
    template <typename ValueType>
    VectorIndexValueIterator<ValueType> MakeVectorIndexValueIterator(const std::vector<ValueType>& container);
}
}

#include "../tcc/StlIndexValueIterator.tcc"
