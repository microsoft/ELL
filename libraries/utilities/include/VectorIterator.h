////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     VectorIterator.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

// stl
#include <vector>

namespace utilities
{
    // A read-only forward iterator for vectors.
    //
    template <typename ValueType>
    class VectorIterator
    {
    public:

        /// <summary> abbreviate iterator type, for improved readability. </summary>
        using StlVectorIteratorType = typename std::vector<ValueType>::const_iterator;

        /// <summary> Ctor. </summary>
        ///
        /// <param name="begin"> The begin. </param>
        /// <param name="end"> The end. </param>
        VectorIterator(const StlVectorIteratorType& begin, const StlVectorIteratorType& end);

        /// <summary> Default copy ctor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        VectorIterator(const VectorIterator&) = default;

        /// <summary> Default move ctor. </summary>
        ///
        /// <param name="parameter1"> [in,out] The first parameter. </param>
        VectorIterator(VectorIterator&&) = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const;

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns the current iterate. </summary>
        ///
        /// <returns> A ValueType. </returns>
        ValueType Get() const;

    protected:

        // members
        StlVectorIteratorType _begin;
        StlVectorIteratorType _end;
    };
}

#include "../tcc/VectorIterator.tcc"
