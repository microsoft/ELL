////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StlIteratorAdapter.h (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <vector>

namespace utilities
{
    /// <summary> An adapter that transforms a pair of STL iterators into a read-only forward iterator with IsValid, Next, and Get functions</summary>
    template <typename IteratorType, typename ValueType> // = decltype(*std::declval<IteratorType>())>
    class StlIteratorAdapter
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
        /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
        StlIteratorAdapter(IteratorType begin, IteratorType end);
        
        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const;

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the value of the current iterate. </summary>
        ///
        /// <returns> The value of the current iterate. </returns>
        ValueType Get() const;

    private:
        IteratorType _current;
        IteratorType _end;
    };

    /// <summary> Convenience function for creating StlIteratorAdapters </summary>
    ///
    /// <param name="begin"> The STL iterator pointing to the beginning of the range to iterate over. </param>
    /// <param name="end"> The STL iterator pointing just past the end of the range to iterate over. </param>
    ///
    /// <returns> A StlIteratorAdapter over the range specified by the begin and end iterators. </returns>
    template <typename IteratorType>
    auto MakeStlIteratorAdapter(IteratorType begin, IteratorType end)->StlIteratorAdapter<IteratorType, decltype(*begin)>;

    /// <summary> Convenience function for creating StlIteratorAdapters </summary>
    ///
    /// <param name="container"> The C++-conforming iterable container to iterate over. </param>
    ///
    /// <returns> A StlIteratorAdapter over the container</returns>
    template <typename ContainerType>
    auto MakeStlIteratorAdapter(ContainerType& container)->StlIteratorAdapter<typename ContainerType::iterator, typename ContainerType::value_type>;
}

#include "../tcc/StlIteratorAdapter.tcc"
