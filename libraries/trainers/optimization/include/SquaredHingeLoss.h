////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquaredHingeLoss.h (optimization)
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
        /// <summary> Implements the Squared Hinge Loss function: loss(prediction, output) = (max(0, 1-prediction*output))^2. </summary>
        class SquaredHingeLoss
        {
        public:
            /// <summary> Checks if an output is compatible with this loss. </summary>
            template <typename OutputType>
            static bool VerifyOutput(OutputType output);

            /// <summary> Returns the smoothness of this loss, which is the Lipschitz coefficient of the loss gradient. </summary>
            constexpr static double Smoothness() { return 1.0; }

            /// <summary> Returns the loss of a scalar prediction, given the true scalar output. </summary>
            ///
            /// <param name="prediction"> The predicted output. </param>
            /// <param name="output"> The true output. </param>
            template <typename OutputType>
            static double Value(double prediction, OutputType output);

            /// <summary> Returns the loss derivative at a given scalar point. </summary>
            ///
            /// <param name="prediction"> The predicted output. </param>
            /// <param name="output"> The true output. </param>
            template <typename OutputType>
            static double Derivative(double prediction, OutputType output);

            /// <summary> Returns the value of the loss conjugate at a given point. </summary>
            ///
            /// <param name="v"> The point at which to evaluate the conjugate. </param>
            /// <param name="output"> The output. </param>
            template <typename OutputType>
            static double Conjugate(double v, OutputType output);

            /// <summary>
            /// Returns the value of the proximal operator of the conjugate of the loss, which is
            ///
            ///     argmin_b {theta*g(b) + (1/2)*(b - a)^2}
            ///
            /// where g() is the convex conjugate of f()
            /// </summary>
            ///
            /// <param name="theta"> The weight of the conjugate function in the proximal operator definition. </param>
            /// <param name="z"> The point at which the proximal operator is evaluated. </param>
            /// <param name="output"> The output. </param>
            template <typename OutputType>
            static double ConjugateProx(double theta, double z, OutputType output);
        };
    } // namespace optimization
} // namespace trainers
} // namespace ell

#include "../tcc/SquaredHingeLoss.tcc"
