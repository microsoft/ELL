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
#include <iostream>
#include <functional>
#include <vector>

namespace linear
{
    /// <summary> Implements a class that references a subvector. </summary>
    class DoubleSubvectorCref : public IVector
    {
    public:

        /// <summary> Constructs a DoubleSubvectorCref. </summary>
        ///
        /// <param name="vec"> The vector to reference. </param>
        /// <param name="offset"> The offset, 0 by default. </param>
        /// <param name="size"> The nmumber of elements in the subvector, the end of the vector by default. </param>
        DoubleSubvectorCref(const DoubleVector& vec, uint64 offset = 0, uint64 size = UINT64_MAX);

        /// <summary> Constructs a DoubleSubvectorCref. </summary>
        ///
        /// <param name="ptr"> A pointer to the first element to reference. </param>
        /// <param name="size"> The number of elements in the subvector. </param>
        DoubleSubvectorCref(const double* ptr, uint64 size);

        /// <summary> Computes the squared 2-norm. </summary>
        ///
        /// <returns> The squared 2-norm. </returns>
        virtual double Norm2() const override;

        /// <summary> Performs the operation: other += scalar * (*this), where other is a std::vector. </summary>
        ///
        /// <param name="other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const override;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> The dot product. </returns>
        virtual double Dot(const double* p_other) const override;

        /// <summary> Prints the vector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream & os) const override;

        /// <summary> Returns the Size of the vector. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const override;

    private:
        const double* _ptr;
        uint64 _size;
    };
}


