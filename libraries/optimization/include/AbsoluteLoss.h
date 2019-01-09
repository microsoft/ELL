////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AbsoluteLoss.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Vector.h>

namespace ell
{
namespace optimization
{
    /// <summary> Implements the Absolute Loss function: loss(prediction, output) = abs(output-prediction) </summary>
    class AbsoluteLoss
    {
    public:
        /// <summary> Checks if an output is compatible with this loss. </summary>
        template <typename OutputType>
        constexpr static bool VerifyOutput(OutputType)
        {
            return true;
        }

        /// <summary> Returns the smoothness of this loss, which is the Lipschitz coefficient of the loss gradient. </summary>
        constexpr static double Smoothness() { return std::numeric_limits<double>::infinity(); }

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
} // namespace ell

#pragma region implementation

#include <cmath>

namespace ell
{
namespace optimization
{
    template <typename OutputType>
    double AbsoluteLoss::Value(double prediction, OutputType output)
    {
        return std::abs(prediction - output);
    }

    template <typename OutputType>
    double AbsoluteLoss::Derivative(double prediction, OutputType output)
    {
        if (prediction == output)
        {
            return 0.0;
        }
        if (prediction < output)
        {
            return -1.0;
        }
        return 1.0;
    }

    template <typename OutputType>
    double AbsoluteLoss::Conjugate(double v, OutputType output)
    {
        if (-1.0 <= v && v <= 1.0)
        {
            return output * v;
        }
        return std::numeric_limits<double>::infinity();
    }

    template <typename OutputType>
    double AbsoluteLoss::ConjugateProx(double theta, double z, OutputType output)
    {
        double a = z - theta * output;

        if (a <= -1.0)
        {
            return -1.0;
        }
        if (a >= 1.0)
        {
            return 1.0;
        }
        return a;
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
