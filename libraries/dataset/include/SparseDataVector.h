////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SparseDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// utilities
#include "CompressedIntegerList.h"

// linear
#include "IndexValue.h"

// stl
#include <cstdint>
#include <type_traits>
#include <vector>

namespace dataset
{
    /// <summary> Implements a sparse vector as an increasing list of indices and their values.
    ///
    /// <typeparam name="ValueType">     Type of the value type. </typeparam>
    /// <typeparam name="tegerListType"> Type of the teger list type. </typeparam>
    template<typename ValueType, typename IntegerListType>
    class SparseDataVector : public IDataVector
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

            /// <summary> \returns The current index-value pair. </summary>
            ///
            /// <returns> An IndexValue. </returns>
            linear::IndexValue Get() const;

        private:
            
            // define typenames to improve readability
            using IndexIteratorType = typename IntegerListType::Iterator;
            using ValueIteratorType = typename std::vector<ValueType>::const_iterator;

            // private ctor, can only be called from SparseDataVector class
            Iterator(const IndexIteratorType& list_iterator, const ValueIteratorType& value_iterator);
            friend SparseDataVector<ValueType, IntegerListType>;

            // members
            IndexIteratorType _index_iterator;
            ValueIteratorType _value_iterator;
        };

        /// <summary> Constructs an empty sparse binary vector. </summary>
        SparseDataVector();

        // Converting constructor
        //

        /// <summary> Constructs an instance of SparseDataVector. </summary>
        ///
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType, typename concept = linear::IsIndexValueIterator<IndexValueIteratorType>>
        SparseDataVector(IndexValueIteratorType IndexValueIterator);

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        SparseDataVector(SparseDataVector<ValueType, IntegerListType>&& other) = default;

        /// <summary> Deleted copy constructor. </summary>
        ///
        /// <param name="other"> The other. </param>
        SparseDataVector(const SparseDataVector<ValueType, IntegerListType>& other) = delete;

        /// <summary> Sets the element at the given index to 1.0. Calls to this function must have a
        /// monotonically increasing argument. The value argument must equal 1.0. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void PushBack(uint64_t index, double value) override;

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
        /// <param name="scalar"> The scalar. </param>
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

    private:
        IntegerListType _indices;
        std::vector<ValueType> _values;
    };

    /// <summary> A sparse double data vector. </summary>
    class SparseDoubleDataVector : public SparseDataVector<double, utilities::CompressedIntegerList>
    {
    public:
        using SparseDataVector<double, utilities::CompressedIntegerList>::SparseDataVector;

        /// <summary> \returns The type of the vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;
    };

    /// <summary> A sparse float data vector. </summary>
    class SparseFloatDataVector : public SparseDataVector<float, utilities::CompressedIntegerList>
    {
    public:
        using SparseDataVector<float, utilities::CompressedIntegerList>::SparseDataVector;

        /// <summary> \returns The type of the vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;
    };

    /// <summary> A sparse short data vector. </summary>
    class SparseShortDataVector : public SparseDataVector<short, utilities::CompressedIntegerList>
    {
    public:
        using SparseDataVector<short, utilities::CompressedIntegerList>::SparseDataVector;

        /// <summary> \returns The type of the vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;
    };
}

#include "../tcc/SparseDataVector.tcc"

