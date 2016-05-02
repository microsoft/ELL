////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     LogLoss.h (lossFunctions)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace lossFunctions
{
    /// <summary> Implements the log-loss functions: loss(x) = log(1 + exp(-x)) </summary>
    class LogLoss
    {
    public:

        /// <summary> Constructs a LogLoss object. </summary>
        ///
        /// <param name="scale"> The loss scale. </param>
        LogLoss(double scale = 1.0);

        /// <summary> Returns the value of the loss at a given point. </summary>
        ///
        /// <param name="prediction"> The prediction. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> The loss. </returns>
        double Evaluate(double prediction, double label) const;

        /// <summary> Returns the value of the loss derivative at a given point. </summary>
        ///
        /// <param name="prediction"> The prediction. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> The loss derivative. </returns>
        double GetDerivative(double prediction, double label) const;

    private:
        double _scale = 1.0;
    };
}
