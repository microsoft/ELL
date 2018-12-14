////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ElasticNetRegularizer.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MatrixSolution.h"
#include "VectorSolution.h"

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        /// <summary> Implements a squared L2 norm regularizer. </summary>
        class ElasticNetRegularizer
        {
        public:
            /// <summary> Constructor. </summary>
            ElasticNetRegularizer(double beta = 1.0) :
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
            /// compute w = argmax_u {v'*u - f(u)} </summary>
            ///
            /// <param name="v"> The point at which the conjugate gradient is computed. </param>
            /// <param name="w"> The output. </param>
            template <typename SolutionType>
            void ConjugateGradient(const SolutionType& v, SolutionType& w) const;

        private:
            double _beta;
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma region implementation

#include "NormProx.h"

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename SolutionType>
        double ElasticNetRegularizer::Value(const SolutionType& w) const
        {
            return 0.5 * Norm2Squared(w) + _beta * w.GetVector().Norm1(); // note: Norm1 does not include the bias term
        }

        template <typename SolutionType>
        double ElasticNetRegularizer::Conjugate(const SolutionType& v) const
        {
            SolutionType w = v;
            L1Prox(w.GetVector(), _beta); // note: L1 term does not apply to the bias term
            double result = -_beta * w.GetVector().Norm1();
            w -= v;
            result += 0.5 * (Norm2Squared(v) - Norm2Squared(w));
            return result;
        }

        template <typename SolutionType>
        void ElasticNetRegularizer::ConjugateGradient(const SolutionType& v, SolutionType& w) const
        {
            w = v;
            L1Prox(w.GetVector(), _beta); // note: L1Prox does not apply to the bias term
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
