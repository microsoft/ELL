////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IVector.h (linear)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Base.h"

// stl
#include <cstdint>
#include <vector>

namespace emll
{
namespace linear
{
    class DoubleVector;

    /// <summary>
    /// Base class for double precision algebraic vectors.
    /// </summary>
    class IVector 
    {
    public:
        /// <summary> Returns the Size of the vector. </summary>
        ///
        /// <returns> The size of the vector. </returns>
        virtual uint64_t Size() const = 0;

        /// <summary> Computes the squared 2-norm. </summary>
        ///
        /// <returns> The squared 2-norm. </returns>
        virtual double Norm2() const = 0;

        /// <summary> Performs the operation: other += scalar * (*this), where other is a std::vector. </summary>
        ///
        /// <param name="other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The scalar. </param>
        void AddTo(std::vector<double>& other, double scalar = 1.0) const;

        /// <summary>
        /// Performs the operation: (*p_other) += scalar * (*this), where other is an array of doubles.
        /// </summary>
        ///
        /// <param name="p_other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double* p_other, double scalar = 1.0) const = 0;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> The dot product. </returns>
        double Dot(const std::vector<double>& other) const;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="p_other"> The other vector. </param>
        ///
        /// <returns> A dot product. </returns>
        virtual double Dot(const double* p_other) const = 0;
    };
}
}
