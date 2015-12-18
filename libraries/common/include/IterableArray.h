// IterableArray.h

#pragma once

#include "types.h"

#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

namespace common
{
    template <typename ValueType>
    class IterableArray : public vector<ValueType>
    {
    public:

        /// A read-only forward iterator for the sparse binary vector.
        ///
        class Iterator
        {
        public:

            /// Default copy ctor
            ///
            Iterator(const Iterator&) = default;

            /// Default move ctor
            ///
            Iterator(Iterator&&) = default;

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
            // abbreviate iterator type, for improved readability 
            using StlIteratorType = typename vector<ValueType>::const_iterator;

            /// private ctor, can only be called from IterableArray class
            Iterator(const StlIteratorType& begin, const StlIteratorType& end);
            friend IterableArray<ValueType>;

            // members
            StlIteratorType _begin;
            StlIteratorType _end;
        };

        /// Ctor
        ///
        IterableArray(uint64 size = 0);
        
        /// Default copy ctor
        ///
        IterableArray(const IterableArray<ValueType>&) = default;
        
        /// Default move ctor
        ///
        IterableArray(IterableArray<ValueType>&&) = default;

        /// \returns The size of the array
        ///
        uint64 Size() const;

        /// \returns An Iterator that points to the beginning of the array.
        ///
        Iterator GetIterator() const;

        /// Prints the array to an output stream
        ///
        void Print(ostream& os) const;
    };

    template <typename ValueType>
    ostream& operator<<(ostream&, const IterableArray<ValueType>&);
}

#include "../tcc/IterableArray.tcc"
