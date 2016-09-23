////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseBinaryDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// utilities
#include "CompressedIntegerList.h"
#include "IntegerList.h"

// linear
#include "IndexValue.h"

// stl
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace emll
{
namespace dataset
{
    /// <summary> Implements a sparse binary vector as an increasing list of the coordinates where the
    /// value is 1.0. </summary>
    ///
    /// <typeparam name="tegerListType"> Type of the teger list type. </typeparam>
    template <typename IntegerListType>
    class SparseBinaryDataVectorBase : public IDataVector
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
            bool IsValid() const { return _list_iterator.IsValid(); }

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next() { _list_iterator.Next(); }

            /// <summary> Returns The current value. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            linear::IndexValue Get() const { return linear::IndexValue{ _list_iterator.Get(), 1.0 }; }

        private:
            /// <summary> define typename to improve readability. </summary>
            using IndexIteratorType = typename IntegerListType::Iterator;

            /// <summary> private ctor, can only be called from SparseBinaryDataVectorBase class. </summary>
            ///
            /// <param name="list_iterator"> The list iterator. </param>
            Iterator(const typename IntegerListType::Iterator& list_iterator);
            friend SparseBinaryDataVectorBase<IntegerListType>;

            // members
            IndexIteratorType _list_iterator;
        };

        SparseBinaryDataVectorBase() = default;

        /// <summary> Converting constructor. </summary>
        ///
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <typeparam name="IIndexValueIterator">  Type of the index value iterator. </typeparam>
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template <typename IndexValueIteratorType, typename concept = linear::IsIndexValueIterator<IndexValueIteratorType>>
        SparseBinaryDataVectorBase(IndexValueIteratorType indexValueIterator);

        SparseBinaryDataVectorBase(SparseBinaryDataVectorBase<IntegerListType>&& other) = default;

        SparseBinaryDataVectorBase(const SparseBinaryDataVectorBase<IntegerListType>& other) = default;

        /// <summary> Sets the element at the given index to 1.0. Calls to this function must have a
        /// monotonically increasing argument. The value argument must equal 1.0. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(size_t index, double value = 1.0) override;

        /// <summary> Returns The largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> An size_t. </returns>
        virtual size_t Size() const override;

        /// <summary> Computes the vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override { return (double)_indices.Size(); }

        /// <summary> Performs (*p_other) += scalar * (*this), where other a dense vector. </summary>
        ///
        /// <param name="p_other"> [in,out] If non-null, the other. </param>
        /// <param name="scalar">  The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// <summary> Computes the Dot product. </summary>
        ///
        /// <param name="p_other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        virtual double Dot(const double* p_other) const override;

        /// <summary> Returns a Iterator that traverses the non-zero entries of the sparse vector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const { return Iterator(_indices.GetIterator()); }

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const override;

    private:
        IntegerListType _indices;
    };

    /// <summary> A sparse binary data vector. </summary>
    class SparseBinaryDataVector : public SparseBinaryDataVectorBase<utilities::CompressedIntegerList>
    {
    public:
        using SparseBinaryDataVectorBase<utilities::CompressedIntegerList>::SparseBinaryDataVectorBase;
    };

    /// <summary> An uncompressed sparse binary data vector. </summary>
    class UncompressedSparseBinaryDataVector : public SparseBinaryDataVectorBase<utilities::IntegerList>
    {
    public:
        using SparseBinaryDataVectorBase<utilities::IntegerList>::SparseBinaryDataVectorBase;
    };
}
}

#include "../tcc/SparseBinaryDataVector.tcc"
