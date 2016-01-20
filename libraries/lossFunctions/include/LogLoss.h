// LogLoss.h

#pragma once

namespace lossFunctions
{
    /// Implements the log-loss functions: loss(x) = log(1 + exp(-x))
    ///
    class LogLoss
    {
    public:

        /// Constructs a LogLoss object
        ///
        LogLoss(double scale = 1.0);

        /// \returns The value of the loss at a given point
        ///
        double Evaluate(double prediction, double label) const;

        /// \returns the value of the loss derivative at a given point
        ///
        double GetDerivative(double prediction, double label) const;

    private:
        double _scale = 1.0;
    };
}