// RealArray.h

#pragma once

#include "types.h"

// stl
#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#include <type_traits>
using std::enable_if;
using std::enable_if_t;
using std::is_base_of;

namespace types
{
    template <typename ValueType>
    class RealArray : public vector<ValueType>
    {
    public:

        /// A read-only forward iterator for the sparse binary vector.
        ///
        class Iterator : public IIndexValueIterator
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
            IndexValue Get() const;

        protected:
            // abbreviate iterator type, for improved readability 
            using StlIteratorType = typename vector<ValueType>::const_iterator;

            /// private ctor, can only be called from RealArray class
            Iterator(const StlIteratorType& begin, const StlIteratorType& end);
            friend RealArray<ValueType>;

            // members
            StlIteratorType _begin;
            StlIteratorType _end;
            uint64 _index = 0;
            void SkipZeros();
        };

        /// Ctor
        ///
        RealArray(uint64 size = 0);

        /// Default copy ctor
        ///
        RealArray(const RealArray<ValueType>&) = default;

        /// Default move ctor
        ///
        RealArray(RealArray<ValueType>&&) = default;

        /// \returns An Iterator that points to the beginning of the array.
        ///
        Iterator GetIterator() const;
    };


    /// A read-only forward iterator for the sparse binary vector.
    ///
    template <typename ValueType>
    class SparseStlIterator : public IIndexValueIterator
    {
    public:
        // abbreviate iterator type, for improved readability
        using StlIteratorType = typename vector<ValueType>::const_iterator;

        /// Default copy ctor
        ///
        SparseStlIterator(const SparseStlIterator&) = default;

        /// Default move ctor
        ///
        SparseStlIterator(SparseStlIterator&&) = default;

        SparseStlIterator(const StlIteratorType& begin, const StlIteratorType& end);

        /// \returns True if the iterator is currently pointing to a valid iterate
       ///
        bool IsValid() const;

        /// Proceeds to the Next iterate
        ///
        void Next();

        /// \returns The current index-value pair
        ///
        IndexValue Get() const;

    protected:

        // members
        StlIteratorType _begin;
        StlIteratorType _end;
        uint64 _index = 0;
        void SkipZeros();
    };




    typedef RealArray<double> DoubleArray;
    typedef RealArray<float> FloatArray;
}

#include "../tcc/RealArray.tcc"
