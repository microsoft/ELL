////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DoubleSubvectorCref.h (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IVector.h"
#include "DoubleVector.h"

// stl
#include <functional>
#include <vector>

namespace linear
{
    /// <summary> Implements a double subvector const reference. </summary>
    class DoubleSubvectorCref : public IVector
    {
    public:

        /// <summary> Constructs a constant reference to a double std::vector. </summary>
        ///
        /// <param name="vec"> The vector. </param>
        /// <param name="offset"> The offset. </param>
        /// <param name="size"> The size. </param>
        DoubleSubvectorCref(const DoubleVector& vec, uint64 offset = 0, uint64 size = UINT64_MAX);

        /// <summary> Constructs a constant reference to a double std::vector. </summary>
        ///
        /// <param name="ptr"> The pointer. </param>
        /// <param name="size"> The size. </param>
        DoubleSubvectorCref(const double* ptr, uint64 size);

        /// <summary> Computes the std::vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override;

        /// <summary>
        /// Performs (*p_other) += scalar * (*this), where other is a dense std::vector.
        /// </summary>
        ///
        /// <param name="p_other"> [in,out] If non-null, the other. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// <summary> Computes the Dot product. </summary>
        ///
        /// <param name="p_other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        virtual double Dot(const double* p_other) const override;

        /// <summary> Prints the std::vector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(ostream & os) const override;

        /// <summary> Returns the Size of the std::vector. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const override;

    private:
        const double* _ptr;
        uint64 _size;
    };
}


