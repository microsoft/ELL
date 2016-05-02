////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     StlIndexValueIterator.h (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexValue.h"

// stl
#include <cstdint>
#include <vector>
#include <type_traits>
#include <utility>

namespace utilities
{
    /// <summary> A read-only forward index-value iterator for an STL container </summary>

    template <typename IteratorType, typename ValueType = decltype(*std::declval<IteratorType>())>
    class StlIndexValueIterator : public linear::IIndexValueIterator
    {
    public:
        /// <summary> Constructs an instance of StlIndexValueIterator. </summary>
        ///
        /// <param name="begin"> STL iterator pointing at beginning of range to iterate over </param>
        /// <param name="end"> STL iterator pointing at end of range to iterate over </param>
        StlIndexValueIterator(const IteratorType& begin, const IteratorType& end);

        /// <summary> \returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const;

        /// <summary> Proceeds to the Next iterate </summary>
        void Next();

        /// <summary> \returns The current index-value pair </summary>
        ///
        /// <returns> The current index-value pair </returns>
        linear::IndexValue Get() const;

    protected:
        IteratorType _begin;
        IteratorType _end;
        uint64_t _index = 0;
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
    VectorIndexValueIterator<ValueType> MakeStlIndexValueIterator(const std::vector<ValueType>& container);


    /// <summary> Copies values from an IndexValueIterator into a std::vector, resizing the vector if necessary </summary>
    ///
    /// <param name="array"> The std::vector to copy values into </param>
    /// <param name="indexValueIterator"> The IndexValueIterator to copy values from </param> 
    template<typename ValueType, typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> = 0>
    void CopyToArray(IndexValueIteratorType& indexValueIterator, std::vector<ValueType>& array);
}


#include "../tcc/StlIndexValueIterator.tcc"
