////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataVector.h"

// utilities
#include "CompressedIntegerList.h"

// linear
#include "IndexValue.h"

// stl
#include <cstdint>
#include <type_traits>
#include <vector>

namespace emll
{
namespace dataset
{
    /// <summary> Implements a sparse vector as an increasing list of indices and their corresponding values.
    ///
    /// <typeparam name="ElementType"> Type of the value type. </typeparam>
    /// <typeparam name="tegerListType"> Type of the teger list type. </typeparam>
    template <typename ElementType, typename IntegerListType>
    class SparseDataVector : public DataVectorBase<SparseDataVector<ElementType, IntegerListType>>
    {
    public:
        /// <summary> A read-only forward iterator for the sparse binary vector. </summary>
        class Iterator : public linear::IIndexValueIterator
        {
        public:
            Iterator(const Iterator&) = default;

            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const { return _index_iterator.IsValid(); }

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> Returns The current index-value pair. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            linear::IndexValue Get() const { return linear::IndexValue{ _index_iterator.Get(), (double)*_value_iterator }; }

        private:
            // define typenames to improve readability
            using IndexIteratorType = typename IntegerListType::Iterator;
            using ValueIteratorType = typename std::vector<ElementType>::const_iterator;

            // private ctor, can only be called from SparseDataVector class
            Iterator(const IndexIteratorType& list_iterator, const ValueIteratorType& value_iterator);
            friend SparseDataVector<ElementType, IntegerListType>;

            // members
            IndexIteratorType _index_iterator;
            ValueIteratorType _value_iterator;
        };

        SparseDataVector() = default;

        SparseDataVector(SparseDataVector<ElementType, IntegerListType>&& other) = default;

        SparseDataVector(const SparseDataVector<ElementType, IntegerListType>& other) = default;

        /// <summary> Constructs an instance of SparseDataVector. </summary>
        ///
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, typename concept = linear::IsIndexValueIterator<IndexValueIteratorType>>
        SparseDataVector(IndexValueIteratorType indexValueIterator);

        /// <summary> Constructs a data vector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        SparseDataVector(std::initializer_list<linear::IndexValue> list);

        /// <summary> Constructs a data vector from an initializer list of indices. </summary>
        ///
        /// <param name="list"> The initializer list of indices where the vector equals 1. </param>
        SparseDataVector(std::initializer_list<size_t> list);

        /// <summary> Returns a Iterator that traverses the non-zero entries of the sparse vector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const;

        /// <summary> Appends an entry to the end of the data vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the entry, must be bigger than the biggest current index. </param>
        /// <param name="value"> The entry value. </param>
        virtual void AppendEntry(size_t index, double value) override;

        /// <summary> Returns The largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> An size_t. </returns>
        virtual size_t Size() const override;

    private:
        IntegerListType _indices;
        std::vector<ElementType> _values;
    };

    // friendly names
    using SparseDoubleDataVector = SparseDataVector<double, utilities::CompressedIntegerList>;
    using SparseFloatDataVector = SparseDataVector<float, utilities::CompressedIntegerList>;
    using SparseShortDataVector = SparseDataVector<short, utilities::CompressedIntegerList>;
    using SparseByteDataVector = SparseDataVector<char, utilities::CompressedIntegerList>;
}
}

#include "../tcc/SparseDataVector.tcc"
