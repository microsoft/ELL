////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DoubleVector.h (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IVector.h"

#include "types.h"
#include "RealArray.h"

namespace linear
{
    /// <summary> A double vector. </summary>
    class DoubleVector : public types::DoubleArray, public IVector
    {
    public:

        /// <summary> Constructs an instance of DoubleVector. </summary>
        ///
        /// <param name="size"> The size. </param>
        DoubleVector(uint64 size = 0);

        /// <summary> Copy constructor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        DoubleVector(const DoubleVector&) = default;

        /// <summary> Move constructor. </summary>
        ///
        /// <param name="parameter1"> [in,out] The first parameter. </param>
        DoubleVector(DoubleVector&&) = default;

        /// <summary> Converting constructor. </summary>
        ///
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <typeparam name="IIndexValueIterator"> Type of the index value iterator. </typeparam>
        /// <typeparam name="dexValueIteratorType"> Type of the dex value iterator type. </typeparam>
        /// <param name="indexValueIterator"> The index value iterator. </param>
        template<typename IndexValueIteratorType, typename concept = enable_if_t<is_base_of<IIndexValueIterator, IndexValueIteratorType>::value>>
        DoubleVector(IndexValueIteratorType indexValueIterator) : types::DoubleArray(indexValueIterator) {}

        using IVector::AddTo;

        /// <summary>
        /// Deletes all of the vector content and sets its Size to zero, but does not deallocate its
        /// memory.
        /// </summary>
        void Reset();

        /// <summary> Computes the vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override;

        /// <summary> Performs (*p_other) += scalar * (*this), where other is a dense vector. </summary>
        ///
        /// <param name="p_other"> [in,out] If non-null, the other. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// <summary> Scales the vector by a scalar. </summary>
        ///
        /// <param name="s"> The double to process. </param>
        void Scale(double s);

        /// <summary> Computes the Dot product. </summary>
        ///
        /// <param name="p_other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        virtual double Dot(const double* p_other) const override;

        /// <summary> Returns the Size of the vector. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const override;

        /// <summary> Prints the vector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(ostream& os) const override;
    };
}



