////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LogLoss.h (functions)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef> // size_t

namespace ell
{
namespace functions
{
    /// <summary> Implements the log-loss functions: loss(x) = log(1 + exp(-x)) </summary>
    class LogLoss
    {
    public:
        /// <summary> Gets the smoothness of this loss, which is the Lipschitz coefficient of the gradient. </summary>
        ///
        /// <returns> The smoothness coefficient. </returns>
        constexpr static double GetSmoothness() { return 0.25; }

        /// <summary> Returns the value of the loss at a given point. </summary>
        ///
        /// <param name="prediction"> The prediction. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> The loss. </returns>
        double operator()(double prediction, double label) const;

        /// <summary> Returns the value of the loss derivative at a given point. </summary>
        ///
        /// <param name="prediction"> The prediction. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> The loss derivative. </returns>
        double GetDerivative(double prediction, double label) const;

        /// <summary> Returns the value of the loss conjugate at a given point. </summary>
        ///
        /// <param name="dual"> The dual variable. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> Value of the loss conjugate. </returns>
        double Conjugate(double dual, double label) const;

        /// <summary>
        /// Returns the value of the conjugate prox function at a given point, which is argmin_b
        /// {sigma*(f*)(b) + (1/2)*(b - a)_2^2}.
        /// </summary>
        ///
        /// <param name="sigma"> The sigma parameter. </param>
        /// <param name="dual"> The dual variable. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> Value of the loss conjugate prox. </returns>
        double ConjugateProx(double sigma, double dual, double label) const;

    private:
        static constexpr double _conjugateBoundary = 1.0e-12;
        static constexpr double _conjugateProxDesiredPrecision = 1.0e-6;
        static constexpr size_t _conjugateProxMaxIterations = 20;
    };
}
}
