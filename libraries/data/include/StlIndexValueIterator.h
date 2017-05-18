////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlIndexValueIterator.h (data)
//  Authors:  Chuck Jacobs, Ofer Dekel
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
    template <IterationPolicy policy, typename IteratorType>
    class StlIndexValueIterator;

    template <typename IteratorType>
    class StlIndexValueIterator<IterationPolicy::skipZeros, IteratorType> : public IIndexValueIterator
    {
    public:
        /// <summary> Constructs an instance of StlIndexValueIterator. </summary>
        ///
        /// <param name="begin"> STL iterator pointing at beginning of range to iterate over. </param>
        /// <param name="end"> STL iterator pointing at end of range to iterate over. </param>
        /// <param name="size"> The number of iterates. </param>
        StlIndexValueIterator(const IteratorType& begin, const IteratorType& end, size_t size);

        /// <summary> Returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const { return _current != _end && _index < _size; }

        /// <summary> Proceeds to the Next iterate </summary>
        void Next();

        /// <summary> Returns The current index-value pair </summary>
        ///
        /// <returns> The current index-value pair </returns>
        IndexValue Get() const { return IndexValue{ _index, (double)*_current }; }

    protected:
        IteratorType _current;
        IteratorType _end;
        size_t _size;
        size_t _index = 0;

        void SkipZeros();
    };

    /// <summary> A read-only forward index-value iterator for an STL container </summary>
    template <typename IteratorType>
    class StlIndexValueIterator<IterationPolicy::all, IteratorType> : public IIndexValueIterator
    {
    public:
        /// <summary> Constructs an instance of StlIndexValueIterator. </summary>
        ///
        /// <param name="begin"> STL iterator pointing at beginning of range to iterate over. </param>
        /// <param name="end"> STL iterator pointing at end of range to iterate over. </param>
        /// <param name="size"> The number of iterates. </param>
        StlIndexValueIterator(const IteratorType& begin, const IteratorType& end, size_t size);

        /// <summary> Returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const { return _index < _size; }

        /// <summary> Proceeds to the Next iterate </summary>
        void Next();

        /// <summary> Returns The current index-value pair </summary>
        ///
        /// <returns> The current index-value pair </returns>
        IndexValue Get() const;

    protected:
        IteratorType _current;
        IteratorType _end;
        size_t _size;
        size_t _index = 0;
    };

    /// <summary> Handy type alias for a StlDenseIterator over a std::vector </summary>
    template <IterationPolicy policy, typename ElementType>
    using VectorIndexValueIterator = StlIndexValueIterator<policy, typename std::vector<ElementType>::const_iterator>;

    /// <summary> Helper function that creates a VectorIndexValueIterator. </summary>
    ///
    /// <typeparam name="policy"> Whether the iterator should skip zeros or not. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <param name="container"> The vector. </param>
    ///
    /// <returns> An IndexValue iterator. </returns>
    template <IterationPolicy policy, typename ElementType>
    VectorIndexValueIterator<policy, ElementType> MakeVectorIndexValueIterator(const std::vector<ElementType>& container);

    /// <summary> Helper function that creates a VectorIndexValueIterator. </summary>
    ///
    /// <typeparam name="policy"> Whether the iterator should skip zeros or not. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <param name="container"> The vector. </param>
    /// <param name="size"> The number of iterates. </param>
    ///
    /// <returns> An IndexValue iterator. </returns>
    template <IterationPolicy policy, typename ElementType>
    VectorIndexValueIterator<policy, ElementType> MakeVectorIndexValueIterator(const std::vector<ElementType>& container, size_t size);
}
}

#include "../tcc/StlIndexValueIterator.tcc"
