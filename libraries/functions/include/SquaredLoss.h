////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquaredLoss.h (functions)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace functions
{
    /// <summary> Implements the squared loss function: loss(x) = x*x. </summary>
    class SquaredLoss
    {
    public:
        /// <summary> Gets the smoothness of this loss, which is the Lipschitz coefficient of the gradient. </summary>
        ///
        /// <returns> The smoothness coefficient. </returns>
        constexpr static double GetSmoothness() { return 1.0; }

        /// <summary> Returns the value of the loss at a given point. </summary>
        ///
        /// <param name="prediction"> The prediction. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> A loss. </returns>
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

        /// <summary> Evaluates the convex function that generates this Bregman loss. </summary>
        ///
        /// <param name="value"> The input to the convex function. </param>
        ///
        /// <returns> The output of the convex function. </returns>
        double BregmanGenerator(double value) const;
    };
}
}
