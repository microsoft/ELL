// RealArray.h

#pragma once

#include "types.h"

// stl
#include <vector>
using std::vector;

#include <iostream>
using std::ostream;

#include <type_traits>
using std::enable_if_t;
using std::is_base_of;

namespace common
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

        /// Converting constructor
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        RealArray(IndexValueIteratorType IndexValueIterator);

        /// Sets the array to zero
        ///
        void Clear();

        /// Sets the enties of the array
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        void Set(IndexValueIteratorType IndexValueIterator);

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
    ostream& operator<<(ostream&, const RealArray<ValueType>&);

    typedef RealArray<double> DoubleArray;
    typedef RealArray<float> FloatArray;
}

#include "../tcc/RealArray.tcc"
