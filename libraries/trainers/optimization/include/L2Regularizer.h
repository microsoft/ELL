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
            /// <summary> Returns the value of the regularizer at a given point. </summary>
            ///
            /// <param name="w"> The point for which the regularizer is computed. </param>
            template <typename SolutionType>
            static double Value(const SolutionType& w);

            /// <summary> Returns the value of the convex conjugate of the regularizer. </summary>
            template <typename SolutionType>
            static double Conjugate(const SolutionType& v);

            /// <summary> Returns the gradient of the conjugate. Namely, Given vector v,
            /// compute w = argmax_u {v'*u - f(u)} = argmin_u {-v'*u + f(u)} </summary>
            ///
            /// <param name="v"> The point at which the conjugate gradient is computed. </param>
            /// <param name="w"> The output. </param>
            template <typename SolutionType>
            static void ConjugateGradient(const SolutionType& v, SolutionType& w);
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell

#include "../tcc/L2Regularizer.tcc"