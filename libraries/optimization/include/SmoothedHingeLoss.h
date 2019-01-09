////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SmoothedHingeLoss.h (optimization)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math/include/Vector.h>

namespace ell
{
namespace optimization
{
    /// <summary> Implements the Huber Hinge Loss function, which is a hinge loss with quadratic smoothing in the interval [1-gamma,1]. </summary>
    class SmoothedHingeLoss
    {
    public:
        /// <summary> Checks if an output is compatible with this loss. </summary>
        template <typename OutputType>
        static bool VerifyOutput(OutputType output);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="gamma"> The inverse smoothness parameter. </param>
        SmoothedHingeLoss(double gamma = 1.0) :
            _gamma(gamma) {}

        /// <summary> Returns the smoothness of this loss, which is the Lipschitz coefficient of the loss gradient. </summary>
        double Smoothness() const { return 2.0 / _gamma; }

        /// <summary> Returns the loss of a scalar prediction, given the true scalar output. </summary>
        ///
        /// <param name="prediction"> The predicted output. </param>
        /// <param name="output"> The true output. </param>
        template <typename OutputType>
        double Value(double prediction, OutputType output) const;

        /// <summary> Returns the loss derivative at a given scalar point. </summary>
        ///
        /// <param name="prediction"> The predicted output. </param>
        /// <param name="output"> The true output. </param>
        template <typename OutputType>
        double Derivative(double prediction, OutputType output) const;

        /// <summary> Returns the value of the loss conjugate at a given point. </summary>
        ///
        /// <param name="v"> The point at which to evaluate the conjugate. </param>
        /// <param name="output"> The output. </param>
        template <typename OutputType>
        double Conjugate(double v, OutputType output) const;

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
        double ConjugateProx(double theta, double z, OutputType output) const;

    private:
        double _gamma;
    };
} // namespace optimization
} // namespace ell

#pragma region implementation

#include "Common.h"

namespace ell
{
namespace optimization
{
    template <typename OutputType>
    bool SmoothedHingeLoss::VerifyOutput(OutputType output)
    {
        if (output == 1.0 || output == -1.0)
        {
            return true;
        }
        return false;
    }

    template <typename OutputType>
    double SmoothedHingeLoss::Value(double prediction, OutputType output) const
    {
        DEBUG_CHECK(VerifyOutput(output), "Smoothed Hinge Loss requires binary outputs");

        double margin = prediction * output;
        if (margin >= 1.0)
        {
            return 0.0;
        }
        if (margin >= 1.0 - _gamma)
        {
            double residual = (prediction - output);
            return 0.5 / _gamma * residual * residual;
        }
        return 1.0 - margin - 0.5 * _gamma;
    }

    template <typename OutputType>
    double SmoothedHingeLoss::Derivative(double prediction, OutputType output) const
    {
        DEBUG_CHECK(VerifyOutput(output), "Smoothed Hinge Loss requires binary outputs");

        double margin = prediction * output;
        if (margin >= 1.0)
        {
            return 0.0;
        }
        if (margin >= 1.0 - _gamma)
        {
            return (prediction - output) / _gamma;
        }
        return -output;
    }

    template <typename OutputType>
    double SmoothedHingeLoss::Conjugate(double v, OutputType output) const
    {
        DEBUG_CHECK(VerifyOutput(output), "Smoothed Hinge Loss requires binary outputs");

        double a = output * v;

        if (-1.0 <= a && a <= 0.0)
        {
            return a + 0.5 * _gamma * v * v;
        }
        return std::numeric_limits<double>::infinity();
    }

    template <typename OutputType>
    double SmoothedHingeLoss::ConjugateProx(double theta, double z, OutputType output) const
    {
        DEBUG_CHECK(VerifyOutput(output), "Smoothed Hinge Loss requires binary outputs");

        double a = output * z;

        if (a < (1.0 - _gamma) * theta - 1.0)
        {
            return -output;
        }
        if (a <= theta)
        {
            return (z - theta * output) / (1 + theta * _gamma);
        }
        return 0.0;
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
