////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DenseDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// types
#include "types.h"
#include "StlIndexValueIterator.h"

// stl
#include <vector>
#include <iostream>
#include <type_traits>

#define DEFAULT_DENSE_VECTOR_CAPACITY 1000

namespace dataset
{
    /// <summary> DenseDataVector Base class. </summary>
    ///
    /// <typeparam name="ValueType"> Type of the value type. </typeparam>
    template<typename ValueType>
    class DenseDataVector : public IDataVector
    {
    public:
        using Iterator = types::VectorIndexValueIterator<ValueType>;

        /// <summary> Constructor. </summary>
        DenseDataVector();

        /// <summary> Constructs an instance of DenseDataVector. </summary>
        ///
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType, typename concept = IsIndexValueIterator<IndexValueIteratorType>>
        DenseDataVector(IndexValueIteratorType IndexValueIterator);

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        DenseDataVector(DenseDataVector&& other) = default;

        /// <summary> Deleted copy constructor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        DenseDataVector(const DenseDataVector&) = delete;

        /// <summary> Sets an entry in the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void PushBack(uint64 index, double value = 1.0) override;

        /// <summary> Deletes all of the std::vector content and sets its Size to zero, but does not
        /// deallocate its memory. </summary>
        virtual void Reset() override;

        /// <summary> \returns The largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const override;

        /// <summary> \returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64 NumNonzeros() const override;

        /// <summary> Computes the std::vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override;

        /// <summary> Performs (*p_other) += scalar * (*this), where other is a dense std::vector. </summary>
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

        /// <summary> \Returns An Iterator that points to the beginning of the std::vector. </summary>
        ///
        /// <returns> The iterator. </returns>
        Iterator GetIterator() const;

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

    private:
        uint64 _num_nonzeros;
        std::vector<ValueType> _data;
    };

    /// <summary> A float data vector. </summary>
    class FloatDataVector : public DenseDataVector<float> 
    {
    public:
        using DenseDataVector<float>::DenseDataVector;

        /// <summary> \returns The type of the std::vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;
    };

    /// <summary> A double data vector. </summary>
    class DoubleDataVector : public DenseDataVector<double>
    {
    public:
        using DenseDataVector<double>::DenseDataVector;

        /// <summary> \returns The type of the std::vector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;
    };
}

#include "../tcc/DenseDataVector.tcc"
