////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     IndexValueIterator.h (types)
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
    class StlIndexValueIterator : public IIndexValueIterator
    {
    public:
        using StlIteratorType = typename std::vector<ValueType>::const_iterator;

        /// <summary> Constructs an instance of StlIndexValueIterator. </summary>
        ///
        /// <param name="begin"> STL iterator pointing at beginning of range to iterate over </param>
        /// <param name="end"> STL iterator pointing at end of range to iterate over </param>
        StlIndexValueIterator(const StlIteratorType& begin, const StlIteratorType& end);

        /// <summary> \returns The number of non-zeros. </summary>
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

    template <typename ValueType>
    StlIndexValueIterator<ValueType> GetStlIndexValueIterator(const std::vector<ValueType>& arr);
}


#include "../tcc/IndexValueIterator.tcc"
