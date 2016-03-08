////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StlIndexValueIteratorAdapter.h (types)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

// stl
#include <vector>

namespace types
{
    /// <summary> A read-only forward index-value iterator for an STL container </summary>

    template <typename ValueType>
    class StlIndexValueIteratorAdapter : public IIndexValueIterator
    {
    public:
        using StlIteratorType = typename std::vector<ValueType>::const_iterator;

        /// <summary> Constructs an instance of StlIndexValueIterator. </summary>
        ///
        /// <param name="begin"> STL iterator pointing at beginning of range to iterate over </param>
        /// <param name="end"> STL iterator pointing at end of range to iterate over </param>
        StlIndexValueIteratorAdapter(const StlIteratorType& begin, const StlIteratorType& end);

        /// <summary> \returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const;

        /// <summary> Proceeds to the Next iterate </summary>
        void Next();

        /// <summary> \returns The current index-value pair </summary>
        ///
        /// <returns> The current index-value pair </returns>
        IndexValue Get() const;

    protected:
        StlIteratorType _begin;
        StlIteratorType _end;
        uint64 _index = 0;
        void SkipZeros();
    };

    /// <summary> Creates an StlIndexValueIteratorAdapter for a std::vector </summary>
    ///
    /// <param name="container"> A std::vector to iterate over </param>
    /// <returns> An StlIndexValueIteratorAdapter that iterates over the given vector </returns>
    template <typename ValueType>
    StlIndexValueIteratorAdapter<ValueType> MakeStlIndexValueIteratorAdapter(const std::vector<ValueType>& container);
}


#include "../tcc/StlIndexValueIteratorAdapter.tcc"
