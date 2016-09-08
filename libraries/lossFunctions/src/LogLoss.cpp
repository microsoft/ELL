////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LogLoss.cpp (lossFunctions)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LogLoss.h"

// stl
#include <cmath>

namespace lossFunctions
{
    LogLoss::LogLoss(double scale)
        : _scale(scale)
    {
    }

    double LogLoss::Evaluate(double prediction, double label) const
    {
        double scaled_margin = _scale * prediction * label;

        if (scaled_margin <= -18.0)
        {
            return -prediction * label;
        }
        else
        {
            double exp_neg_scaled_margin = std::exp(-scaled_margin);
            return std::log(1.0 + exp_neg_scaled_margin) / _scale;
        }
    }

    double LogLoss::GetDerivative(double prediction, double label) const
    {
        double scaled_margin = prediction * label * _scale;

        if (scaled_margin <= 0.0)
        {
            double exp_scaled_margin = std::exp(scaled_margin);
            return -label / (1.0 + exp_scaled_margin);
        }
        else
        {
            double exp_neg_scaled_margin = std::exp(-scaled_margin);
            return -label * exp_neg_scaled_margin / (1.0 + exp_neg_scaled_margin);
        }
    }
}
