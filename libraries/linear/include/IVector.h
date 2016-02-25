////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     IVector.h (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

#include "Base.h"

// stl
#include <vector>

namespace linear
{
    /// <summary>
    /// Base class for infinite-dimensional vectors of double numbers. Each implementation of this
    /// class has a mathematical dimension of infinity and includes an explicitly specified prefix
    /// followed by an implicit suffix of zeros.
    /// </summary>
    class IVector : public Base
    {
    public:

        /// <summary> Returns the Size of the std::vector. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const =0;

        /// <summary> Computes the std::vector squared 2-norm. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const =0;

        /// <summary> Performs other += scalar * (*this), where other is a dense std::vector. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        /// <param name="scalar"> The scalar. </param>
        void AddTo(std::vector<double>& other, double scalar = 1.0) const;

        /// <summary>
        /// Performs (*p_other) += scalar * (*this), where other is a dense std::vector.
        /// </summary>
        ///
        /// <param name="p_other"> [in,out] If non-null, the other. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const = 0;

        /// <summary> Computes the vector Dot product. </summary>
        ///
        /// <param name="other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        double Dot(const std::vector<double>& other) const;

        /// <summary> Computes the vector Dot product. </summary>
        ///
        /// <param name="p_other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        virtual double Dot(const double* p_other) const = 0;
    };
}
