////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StlReferenceIterator.h (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <vector>

namespace emll
{
namespace utilities
{
    /// <summary> An adapter that transforms a pair of STL iterators into a read-only forward iterator with IsValid, Next, and Get functions</summary>
    template <typename IteratorType, typename ValueType = typename std::decay<decltype(*std::declval<IteratorType>())>::type>
    class StlReferenceIterator
    {
    public:

        StlReferenceIterator() = default;

        /// <summary> Constructor </summary>
        ///
        /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
        /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
        StlReferenceIterator(IteratorType begin, IteratorType end);

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const { return _current != _end; }

        /// <summary> Returns true if the iterator knows its size. </summary>
        ///
        /// <returns> true if NumIteratesLeft returns a valid number, false if not. </returns>
        bool HasSize() const { return true; }

        /// <summary>
        /// Returns the number of iterates left in this iterator, including the current one.
        /// </summary>
        ///
        /// <returns> The total number of iterates left. </returns>
        uint64_t NumIteratesLeft() const { return _end - _current; }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the value of the current iterate. </summary>
        ///
        /// <returns> The value of the current iterate. </returns>
        const ValueType& Get() const { return *_current; }

    protected:
        IteratorType _current;
        IteratorType _end;
    };

    /// <summary> Handy type alias for a StlReferenceIterator over a std::vector </summary>
    template <typename ValueType>
    using VectorReferenceIterator = StlReferenceIterator<typename std::vector<ValueType>::const_iterator>;

    /// <summary> Convenience function for creating StlReferenceIterators </summary>
    ///
    /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
    /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
    ///
    /// <returns> A StlReferenceIterator over the range specified by the begin and end iterators. </returns>
    template <typename IteratorType>
    auto MakeStlReferenceIterator(IteratorType begin, IteratorType end) -> StlReferenceIterator<IteratorType>;

    /// <summary> Convenience function for creating StlReferenceIterators </summary>
    ///
    /// <param name="container"> The C++-conforming iterable container to iterate over. </param>
    ///
    /// <returns> A StlReferenceIterator over the container</returns>
    template <typename ContainerType>
    auto MakeStlReferenceIterator(ContainerType& container) -> StlReferenceIterator<typename ContainerType::iterator, typename ContainerType::value_type>;
}
}

#include "../tcc/StlReferenceIterator.tcc"
