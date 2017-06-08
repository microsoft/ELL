////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquaredLoss.cpp (functions)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SquaredLoss.h"

// stl
#include <cmath>

namespace ell
{
namespace functions
{
    double SquaredLoss::operator()(double prediction, double label) const
    {
        double residual = prediction - label;
        return 0.5 * residual * residual;
    }

    double SquaredLoss::GetDerivative(double prediction, double label) const
    {
        return prediction - label;
    }

    double SquaredLoss::Conjugate(double dual, double label) const
    {
        return (0.5 * dual + label) * dual;
    }

    double SquaredLoss::ConjugateProx(double sigma, double dual, double label) const
    {
        return (dual - sigma * label) / (1 + sigma);
    }

    double SquaredLoss::BregmanGenerator(double value) const
    {
        return value * value;
    }
}
}
