////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MaxRegularizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Vector.h"

// stl
#include <vector>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        /// <summary> Implements a max-norm regularizer. </summary>
        class MaxRegularizer
        {
        public:
            /// <summary> Constructor. </summary>
            MaxRegularizer(double beta = 1.0) :
                _beta(beta) {}

            /// <summary> Returns the value of the regularizer at a given point. </summary>
            ///
            /// <param name="w"> The point for which the regularizer is computed. </param>
            template <typename SolutionType>
            double Value(const SolutionType& w) const;

            /// <summary> Returns the value of the convex conjugate of the regularizer. </summary>
            ///
            template <typename SolutionType>
            double Conjugate(const SolutionType& v) const;

            /// <summary> Returns the gradient of the conjugate. Namely, Given vector v,
            /// compute w = argmax_u {v'*u - f(u)} = argmin_u {-v'*u + f(u)} </summary>
            ///
            /// <param name="v"> The point at which the conjugate gradient is computed. </param>
            /// <param name="w"> The output. </param>
            template <typename SolutionType>
            void ConjugateGradient(const SolutionType& v, SolutionType& w) const;

        private:
            double _beta;
            mutable std::vector<size_t> _scratch;
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell

#include "../tcc/MaxRegularizer.tcc"
