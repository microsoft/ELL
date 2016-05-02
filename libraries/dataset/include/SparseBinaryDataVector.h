////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     SparseBinaryDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
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
#include <type_traits>
#include <iostream>

namespace dataset
{
    /// <summary> Implements a sparse binary vector as an increasing list of the coordinates where the
    /// value is 1.0. </summary>
    ///
    /// <typeparam name="tegerListType"> Type of the teger list type. </typeparam>
    template<typename IntegerListType>
    class SparseBinaryDataVectorBase : public IDataVector
    {
    public:

        /// <summary> A read-only forward iterator for the sparse binary vector. </summary>
        class Iterator : public linear::IIndexValueIterator
        {
        public:

            /// <summary> Default copy ctor. </summary>
            ///
            /// <param name="parameter1"> The first parameter. </param>
            Iterator(const Iterator&) = default;

            /// <summary> Default move ctor. </summary>
            ///
            /// <param name="parameter1"> [in,out] The first parameter. </param>
            Iterator(Iterator&&) = default;

            /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
            ///
            /// <returns> true if it succeeds, false if it fails. </returns>
            bool IsValid() const;

            /// <summary> Proceeds to the Next iterate. </summary>
            void Next();

            /// <summary> \returns The current value. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            linear::IndexValue Get() const;

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

        /// <summary> Constructs an empty sparse binary vector. </summary>
        SparseBinaryDataVectorBase();

        /// <summary> Converting constructor. </summary>
        ///
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <typeparam name="IIndexValueIterator">  Type of the index value iterator. </typeparam>
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType, typename concept = linear::IsIndexValueIterator<IndexValueIteratorType>>
        SparseBinaryDataVectorBase(IndexValueIteratorType indexValueIterator);

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        SparseBinaryDataVectorBase(SparseBinaryDataVectorBase<IntegerListType>&& other) = default;

        /// <summary> Deleted copy constructor. </summary>
        ///
        /// <param name="other"> The other. </param>
        SparseBinaryDataVectorBase(const SparseBinaryDataVectorBase<IntegerListType>& other) = default;

        /// <summary> Get the type of the vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;

        /// <summary> Sets the element at the given index to 1.0. Calls to this function must have a
        /// monotonically increasing argument. The value argument must equal 1.0. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(uint64_t index, double value = 1.0) override;

        /// <summary> Deletes all of the vector content and sets its Size to zero, but does not deallocate
        /// its memory. </summary>
        virtual void Reset() override;

        /// <summary> \returns The largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> An uint64_t. </returns>
        virtual uint64_t Size() const override;

        /// <summary> \returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64_t NumNonzeros() const override;

        /// <summary> Computes the vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override;

        /// <summary> Performs (*p_other) += scalar * (*this), where other a dense vector. </summary>
        ///
        /// <param name="p_other"> [in,out] If non-null, the other. </param>
        /// <param name="scalar">  The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;
        using IVector::AddTo;

        /// <summary> Computes the Dot product. </summary>
        ///
        /// <param name="p_other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        virtual double Dot(const double* p_other) const override;
        using IVector::Dot;

        /// <summary> \Returns a Iterator that traverses the non-zero entries of the sparse vector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const;

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

        /// <summary> Makes a deep copy of the datavector </summary>
        ///
        /// <returns> A deep copy of the datavector </summary>
        virtual std::unique_ptr<IDataVector> Clone() const override;

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray(uint64_t size) const override;

    private:
        IntegerListType _indices;
    };

    /// <summary> A sparse binary data vector. </summary>
    class  SparseBinaryDataVector : public SparseBinaryDataVectorBase<utilities::CompressedIntegerList>
    {
    public:
        using SparseBinaryDataVectorBase<utilities::CompressedIntegerList>::SparseBinaryDataVectorBase;

        /// <summary> \returns The type of the vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;
    };

    /// <summary> An uncompressed sparse binary data vector. </summary>
    class UncompressedSparseBinaryDataVector : public SparseBinaryDataVectorBase<utilities::IntegerList>
    {
    public:
        using SparseBinaryDataVectorBase<utilities::IntegerList>::SparseBinaryDataVectorBase;

        /// <summary> \returns The type of the vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;
    };
}

#include "../tcc/SparseBinaryDataVector.tcc"


