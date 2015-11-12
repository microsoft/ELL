// LogLoss.h

#pragma once

namespace loss_functions
{
    class LogLoss
    {

    public:

        /// Constructs a LogLoss object
        ///
        LogLoss(double Scale = 1.0);

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