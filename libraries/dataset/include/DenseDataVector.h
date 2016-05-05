////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// utilities
#include "StlIndexValueIterator.h"

// stl
#include <cstdint>
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
        using Iterator = utilities::VectorIndexValueIterator<ValueType>;

        /// <summary> Constructor. </summary>
        DenseDataVector();

        /// <summary> Constructs an instance of DenseDataVector. </summary>
        ///
        /// <param name="IndexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType, typename concept = linear::IsIndexValueIterator<IndexValueIteratorType>>
        DenseDataVector(IndexValueIteratorType indexValueIterator);

        /// <summary> Copy constructor. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        DenseDataVector(const DenseDataVector&) = default; 

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        DenseDataVector(DenseDataVector&& other) = default;

        /// <summary> Constructs an instance of DenseDataVector from a std::vector. </summary>
        ///
        /// <param name="data"> The std::vector. </param>
        DenseDataVector(std::vector<ValueType> data);
        
        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the desired element. </param>
        ///
        /// <returns> Value of the desired element. </returns>
        double operator[](uint64_t index) const;

        /// <summary> Sets an entry in the std::vector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void AppendEntry(uint64_t index, double value = 1.0) override;

        /// <summary> Deletes all of the std::vector content and sets its Size to zero, but does not
        /// deallocate its memory. </summary>
        virtual void Reset() override;

        /// <summary> \returns The largest index of a non-zero entry plus one. </summary>
        ///
        /// <returns> An uint64_t. </returns>
        virtual uint64_t Size() const override;

        /// <summary> \returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64_t NumNonzeros() const override;

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
        
        /// <summary> Makes a deep copy of the datavector </summary>
        ///
        /// <returns> A deep copy of the datavector </summary>
        virtual std::unique_ptr<IDataVector> Clone() const override;

        /// <summary> Copies the contents of this DataVector into a double array of given size. </summary>
        ///
        /// <returns> The array. </returns>
        virtual std::vector<double> ToArray() const override;
        
        ///// <summary> \returns The type of the std::vector. </summary>
        /////
        ///// <returns> The type. </returns>
        virtual type GetType() const override;

    private:
        uint64_t _numNonzeros;
        std::vector<ValueType> _data;
    };

    /// <summary> A float data vector. </summary>
    class FloatDataVector : public DenseDataVector<float> 
    {
    public:
        using DenseDataVector<float>::DenseDataVector;

        /// <summary> \returns The type of the std::vector. </summary>
        ///
        /// 
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
