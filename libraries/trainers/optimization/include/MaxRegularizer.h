////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MaxRegularizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Vector.h>

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

#pragma region implementation

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename SolutionType>
        double MaxRegularizer::Value(const SolutionType& w) const
        {
            return 0.5 * Norm2Squared(w) + _beta * w.GetVector().NormInfinity(); // note: NormInfinity does not include the bias term
        }

        template <typename SolutionType>
        double MaxRegularizer::Conjugate(const SolutionType& v) const
        {
            SolutionType w = v;
            LInfinityProx(w.GetVector(), _scratch, _beta); // note: LInfinity term does not apply to the bias term
            double result = -_beta * w.GetVector().NormInfinity();
            w -= v;
            result += 0.5 * (Norm2Squared(v) - Norm2Squared(w));
            return result;
        }

        template <typename SolutionType>
        void MaxRegularizer::ConjugateGradient(const SolutionType& v, SolutionType& w) const
        {
            w = v;
            LInfinityProx(w.GetVector(), _scratch, _beta); // note: LInfinityProx does not apply to the bias term
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
