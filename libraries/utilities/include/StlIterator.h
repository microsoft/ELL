////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     StlIterator.h (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>
#include <cstdint>

namespace utilities
{
    /// <summary> An adapter that transforms a pair of STL iterators into a read-only forward iterator with IsValid, Next, and Get functions</summary>
    template <typename IteratorType, typename ValueType = typename std::decay<decltype(*std::declval<IteratorType>())>::type>
    class StlIterator
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
        /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
        StlIterator(IteratorType begin, IteratorType end);

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const;

        /// <summary> Returns true if the iterator knows its size. </summary>
        ///
        /// <returns> true if NumIteratesLeft returns a valid number, false if not. </returns>
        bool HasSize() const;

        /// <summary>
        /// Returns the number of iterates left in this iterator, including the current one.
        /// </summary>
        ///
        /// <returns> The total number of iterates left. </returns>
        uint64_t NumIteratesLeft() const;

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the value of the current iterate. </summary>
        ///
        /// <returns> The value of the current iterate. </returns>
        const ValueType& Get() const;

    private:
        IteratorType _current;
        IteratorType _end;
    };

    /// <summary> Handy type alias for a StlIterator over a std::vector </summary>
    template <typename ValueType>
    using VectorIterator = StlIterator<typename std::vector<ValueType>::const_iterator>;

    /// <summary> Convenience function for creating StlIterators </summary>
    ///
    /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
    /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
    ///
    /// <returns> A StlIterator over the range specified by the begin and end iterators. </returns>
    template <typename IteratorType>
    auto MakeStlIterator(IteratorType begin, IteratorType end)->StlIterator<IteratorType>;

    /// <summary> Convenience function for creating StlIterators </summary>
    ///
    /// <param name="container"> The C++-conforming iterable container to iterate over. </param>
    ///
    /// <returns> A StlIterator over the container</returns>
    template <typename ContainerType>
    auto MakeStlIterator(ContainerType& container)->StlIterator<typename ContainerType::iterator, typename ContainerType::value_type>;
}

#include "../tcc/StlIterator.tcc"
