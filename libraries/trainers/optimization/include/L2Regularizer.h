////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     L2Regularizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Vector.h"

namespace ell
{
namespace trainers
{
namespace optimization
{
    /// <summary> Implements a squared L2 norm regularizer. </summary>
    class L2Regularizer
    {
    public:
        /// <summary> Computes the value of the regularizer at a given point. </summary>
        ///
        /// <param name="w"> The point for which the regularizer is computed. </param>
        ///
        /// <returns> Value of the regularizer. </returns>
        template <typename SolutionType>
        static double Value(const SolutionType& w);

        /// <summary> Computes the value of the convex conjugate of the regularizer. </summary>
        ///
        /// <param name="v"> The point at which the conjugate is computed. </param>
        /// <returns> Value of the conjugate. </returns>
        template <typename SolutionType>
        static double Conjugate(const SolutionType& v);

        /// <summary> Computes the conjugate gradient function. Namely, Given vector v, 
        /// compute w = argmax_u {v'*u - f(u)} = argmin_u {-v'*u + f(u)} </summary>
        ///
        /// <param name="v"> The point at which the conjugate gradient is computed. </param>
        /// <param name="w"> The output. </param>
        template <typename SolutionType>
        static void ConjugateGradient(const SolutionType& v, SolutionType& w);
    };
}
}
}

#include "../tcc/L2Regularizer.tcc"