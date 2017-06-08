////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HingeLoss.cpp (functions)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HingeLoss.h"

namespace ell
{
namespace functions
{
    double HingeLoss::operator()(double prediction, double label) const
    {
        double scaled_margin = prediction * label;

        if (scaled_margin <= 1.0)
        {
            return 1.0 - scaled_margin;
        }
        else
        {
            return 0.0;
        }
    }

    double HingeLoss::GetDerivative(double prediction, double label) const
    {
        if (prediction * label <= 1.0)
        {
            return -label;
        }
        else
        {
            return 0.0;
        }
    }
}
}
