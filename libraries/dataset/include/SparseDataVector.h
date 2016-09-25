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

        SparseDataVector(const SparseDataVector<ElementType, IntegerListType>& other) = delete;

        /// <summary> Constructs a sparse data vector from an index value iterator. </summary>
        ///
        /// <typeparam name="IndexValueIteratorType"> Type of index value iterator. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> Concept = true>
        SparseDataVector(IndexValueIteratorType indexValueIterator);

        /// <summary> Constructs a sparse data vector by copying the contents of another data vector. </summary>
        ///
        /// <typeparam name="DataVectorType"> Type of the vector being copied. </typeparam>
        /// <param name="dataVector"> The data vector being copied. </param>
        template <typename DataVectorType, IsDataVector<DataVectorType> Concept = true>
        SparseDataVector(DataVectorType dataVector);

        /// <summary> Constructs a data vector from an initializer list of index value pairs. </summary>
        ///
        /// <param name="list"> The initializer list. </param>
        SparseDataVector(std::initializer_list<linear::IndexValue> list);

        /// <summary> Constructs a data vector from an initializer list of values. </summary>
        ///
        /// <param name="list"> The initializer list of values. </param>
        SparseDataVector(std::initializer_list<double> list);

        /// <summary> Returns a Iterator that traverses the non-zero entries of the sparse vector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const;

        /// <summary> Appends an element to the end of the data vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the element, must be bigger than the biggest current index. </param>
        /// <param name="value"> The element value. </param>
        virtual void AppendElement(size_t index, double value) override;

        /// <summary> Returns The largest index of a non-zero element plus one. </summary>
        ///
        /// <returns> An size_t. </returns>
        virtual size_t Size() const override;

    private:
        IntegerListType _indices;
        std::vector<ElementType> _values;
    };

    /// <summary> A sparse data vector with double elements. </summary>
    struct SparseDoubleDataVector : public SparseDataVector<double, utilities::CompressedIntegerList>
    {
        using SparseDataVector<double, utilities::CompressedIntegerList>::SparseDataVector;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::SparseDoubleDataVector; }
    };

    /// <summary> A sparse data vector with float elements. </summary>
    struct SparseFloatDataVector : public SparseDataVector<float, utilities::CompressedIntegerList>
    {
        using SparseDataVector<float, utilities::CompressedIntegerList>::SparseDataVector;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::SparseFloatDataVector; }
    };

    /// <summary> A sparse data vector with short elements. </summary>
    struct SparseShortDataVector : public SparseDataVector<short, utilities::CompressedIntegerList>
    {
        using SparseDataVector<short, utilities::CompressedIntegerList>::SparseDataVector;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::SparseShortDataVector; }
    };

    /// <summary> A sparse data vector with byte elements. </summary>
    struct SparseByteDataVector : public SparseDataVector<char, utilities::CompressedIntegerList>
    {
        using SparseDataVector<char, utilities::CompressedIntegerList>::SparseDataVector;

        /// <summary> Gets the data vector type. </summary>
        ///
        /// <returns> The data vector type. </returns>
        virtual IDataVector::Type GetType() const override { return IDataVector::Type::SparseByteDataVector; }
    };
}
}

#include "../tcc/SparseDataVector.tcc"
