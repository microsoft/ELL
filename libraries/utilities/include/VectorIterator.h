// VectorIterator.h

#pragma once

#include "types.h"

#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

namespace utilities
{
    /// A read-only forward iterator for vectors.
    ///
    template <typename ValueType>
    class VectorIterator
    {
    public:

        // abbreviate iterator type, for improved readability 
        using StlVectorIteratorType = typename vector<ValueType>::const_iterator;

        /// Ctor
        ///
        VectorIterator(const StlVectorIteratorType& begin, const StlVectorIteratorType& end);

        /// Default copy ctor
        ///
        VectorIterator(const VectorIterator&) = default;

        /// Default move ctor
        ///
        VectorIterator(VectorIterator&&) = default;

        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        bool IsValid() const;

        /// Proceeds to the Next iterate
        ///
        void Next();

        /// \returns The current index-value pair
        ///
        ValueType Get() const;

    protected:

        // members
        StlVectorIteratorType _begin;
        StlVectorIteratorType _end;
    };
}

#include "../tcc/VectorIterator.tcc"
