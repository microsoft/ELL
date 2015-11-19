// SquaredLoss.h

#pragma once

namespace loss_functions
{
	/// Implements the squared loss function: loss(x) = x*x
	///
    class SquaredLoss
    {
    public:

        /// \returns The value of the loss at a given point
        ///
        double Evaluate(double prediction, double label) const;

        /// \returns the value of the loss derivative at a given point
        ///
        double GetDerivative(double prediction, double label) const;
    };
}