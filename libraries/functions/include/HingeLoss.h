////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HingeLoss.h (functions)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace functions
{
    /// <summary> Implements the hinge loss function: loss(x) = max(1-x, 0) </summary>
    class HingeLoss
    {

    public:
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
    };
}
}
