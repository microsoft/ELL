////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SmoothHingeLoss.h (functions)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace functions
{
    /// <summary> Implements the hinge loss function: loss(x) = max(1-x, 0) </summary>
    class SmoothHingeLoss
    {
    public:
        /// <summary> Gets the smoothness of this loss, which is the Lipschitz coefficient of the gradient. </summary>
        ///
        /// <returns> The smoothness coefficient. </returns>
        double GetSmoothness() const { return 1.0/_inverseSmoothness; }

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
        double _inverseSmoothness = 1.0;      // _inverseSmoothness=0 reduces to the standard hinge loss. Must be nonnegative. The bigger, the smoother.
    };
}
}