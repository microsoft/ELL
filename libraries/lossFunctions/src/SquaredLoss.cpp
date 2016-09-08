////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SquaredLoss.cpp (lossFunctions)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SquaredLoss.h"

// stl
#include <cmath>

namespace lossFunctions
{
    double SquaredLoss::Evaluate(double prediction, double label) const
    {
        double residual = prediction - label;
        return 0.5 * residual * residual;
    }

    double SquaredLoss::GetDerivative(double prediction, double label) const
    {
        double residual = prediction - label;
        return residual;
    }

    double SquaredLoss::BregmanGenerator(double value) const
    {
        return value * value;
    }
}
