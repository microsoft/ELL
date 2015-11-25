// RealArray.h

#pragma once

#include "types.h"

#include <vector>
using std::vector;

namespace common
{
    template <typename RealType>
    class RealArray : public vector<RealType>
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
            indexValue Get() const;

        private:

            // abbreviate iterator type, for improved readability 
            using StlIteratorType = typename vector<RealType>::const_iterator;

            /// private ctor, can only be called from RealArray class
            Iterator(const StlIteratorType& begin, const StlIteratorType& end);
            friend RealArray<RealType>;

            // members
            StlIteratorType _begin;
            StlIteratorType _end;
            uint64 _index = 0;
            void SkipZeros();
        };


        RealArray(uint64 size);
        RealArray(const RealArray<RealType>&) = default;
        RealArray(RealArray<RealType>&&) = default;


        /// Converting constructor
        ///
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        RealArray(IndexValueIteratorType indexValueIterator);

        /// \Returns a Iterator that points to the beginning of the list.
        ///
        Iterator GetIterator() const;

        /// Prints the array to an output stream
        ///
        virtual void Print(ostream & os) const;
    };

    template <typename RealType>
    ostream& operator<<(ostream&, const RealArray<RealType>&);
}

#include "../tcc/RealArray.tcc"
