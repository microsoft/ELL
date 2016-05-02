////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SquaredLoss.h (lossFunctions)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace lossFunctions
{
    /// <summary> Implements the squared loss function: loss(x) = x*x. </summary>
    class SquaredLoss
    {
    public:

        /// <summary> Returns the value of the loss at a given point. </summary>
        ///
        /// <param name="prediction"> The prediction. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> A loss. </returns>
        double Evaluate(double prediction, double label) const;

        /// <summary> Returns the value of the loss derivative at a given point. </summary>
        ///
        /// <param name="prediction"> The prediction. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> The loss derivative. </returns>
        double GetDerivative(double prediction, double label) const;
    };
}
