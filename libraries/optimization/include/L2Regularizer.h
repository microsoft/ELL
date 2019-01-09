////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     L2Regularizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Vector.h>

namespace ell
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
} // namespace ell

#pragma region implementation

namespace ell
{
namespace optimization
{
    template <typename SolutionType>
    double L2Regularizer::Value(const SolutionType& w)
    {
        return 0.5 * Norm2Squared(w);
    }

    template <typename SolutionType>
    double L2Regularizer::Conjugate(const SolutionType& v)
    {
        return 0.5 * Norm2Squared(v);
    }

    template <typename SolutionType>
    void L2Regularizer::ConjugateGradient(const SolutionType& v, SolutionType& w)
    {
        w = v;
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
