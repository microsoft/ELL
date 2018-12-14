////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LogisticLoss.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Vector.h>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        /// <summary> Implements the Logistic Loss function: loss(prediction, output) = log(1 + exp(-prediction*output)). </summary>
        class LogisticLoss
        {
        public:
            /// <summary> Checks if an output is compatible with this loss. </summary>
            template <typename OutputType>
            static bool VerifyOutput(OutputType output);

            /// <summary> Returns the smoothness of this loss, which is the Lipschitz coefficient of the loss gradient. </summary>
            constexpr static double Smoothness() { return 0.25; }

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

#pragma region implementation

#include "Common.h"

#include <cmath>

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename OutputType>
        bool LogisticLoss::VerifyOutput(OutputType output)
        {
            if (output == 1.0 || output == -1.0)
            {
                return true;
            }
            return false;
        }

        template <typename OutputType>
        double LogisticLoss::Value(double prediction, OutputType output)
        {
            DEBUG_CHECK(VerifyOutput(output), "Logistic Loss requires binary outputs");

            const double exponentLimit = 18.0;

            double margin = prediction * output;

            if (margin <= -exponentLimit)
            {
                return -margin;
            }
            return std::log1p(std::exp(-margin));
        }

        template <typename OutputType>
        double LogisticLoss::Derivative(double prediction, OutputType output)
        {
            DEBUG_CHECK(VerifyOutput(output), "Logistic Loss requires binary outputs");

            double margin = static_cast<double>(prediction * output);

            if (margin <= 0.0)
            {
                return -output / (1 + std::exp(margin));
            }
            auto expNegMargin = std::exp(-margin);
            return -output * expNegMargin / (1 + expNegMargin);
        }

        template <typename OutputType>
        double LogisticLoss::Conjugate(double v, OutputType output)
        {
            DEBUG_CHECK(VerifyOutput(output), "Logistic Loss requires binary outputs");

            const double conjugateBoundary = 1.0e-12;
            double a = output * v;

            if (a < -1.0 || a > 0.0)
            {
                return std::numeric_limits<double>::infinity();
            }
            if (a <= conjugateBoundary - 1.0 || -conjugateBoundary <= a)
            {
                return 0.0;
            }
            return (1.0 + a) * std::log1p(a) + (-a) * std::log(-a);
        }

        template <typename OutputType>
        double LogisticLoss::ConjugateProx(double theta, double z, OutputType output)
        {
            DEBUG_CHECK(VerifyOutput(output), "Logistic Loss requires binary outputs");

            const double conjugateBoundary = 1.0e-12;
            const double conjugateProxDesiredDualityGap = 1.0e-6;
            const size_t conjugateProxMaxIterations = 20;

            double lowerBound = conjugateBoundary - 1.0;
            double upperBound = -conjugateBoundary;

            double a = output * z;
            double b = std::min(upperBound, std::max(lowerBound, a));
            double f = 0, df = 0;
            for (size_t k = 0; k < conjugateProxMaxIterations; ++k)
            {
                f = b - a + theta * log((1.0 + b) / (-b));
                if (std::abs(f) <= conjugateProxDesiredDualityGap) break;
                df = 1.0 - theta / (b * (1.0 + b));
                b -= f / df;
                b = std::min(upperBound, std::max(lowerBound, b));
            }
            return b * output;
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell

#pragma endregion implementation
