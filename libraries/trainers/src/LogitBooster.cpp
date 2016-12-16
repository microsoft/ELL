////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LogitBooster.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LogitBooster.h"

#include <cmath>

namespace ell
{
namespace trainers
{
    LogitBooster::LogitBooster(double scale)
        : _scale(scale)
    {
    }

    data::WeightLabel LogitBooster::GetWeakWeightLabel(const data::WeightLabel& strongWeightLabel, double prediction) const
    {
        double scaledMargin = _scale * strongWeightLabel.label * prediction;
        if (scaledMargin > 0)
        {
            double expNegativeScaledMargin = exp(-scaledMargin);
            return { strongWeightLabel.weight * expNegativeScaledMargin / (1.0 + expNegativeScaledMargin), strongWeightLabel.label };
        }
        else
        {
            return { strongWeightLabel.weight / (1.0 + exp(scaledMargin)), strongWeightLabel.label };
        }
    }
}
}
