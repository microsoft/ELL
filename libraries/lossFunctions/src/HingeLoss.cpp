////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     HingeLoss.cpp (lossFunctions)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HingeLoss.h"
#include "ELL_HingeLoss.h"

namespace emll
{
namespace lossFunctions
{
    double HingeLoss::Evaluate(double prediction, double label) const
    {
        double scaled_margin = prediction * label;

        if (scaled_margin <= 1.0)
        {
            return 1.0 - scaled_margin;
        }

        return 0.0;
    }

    double HingeLoss::GetDerivative(double prediction, double label) const
    {
        if (prediction * label <= 1.0)
        {
            return -label;
        }

        return 0.0;
    }
}
}

double ELL_HingeLoss::Evaluate(double prediction, double label)
{
    emll::lossFunctions::HingeLoss h;
    return h.Evaluate(prediction, label);
}

double ELL_HingeLoss::GetDerivative(double prediction, double label)
{
    emll::lossFunctions::HingeLoss h;
    return h.GetDerivative(prediction, label);
}
