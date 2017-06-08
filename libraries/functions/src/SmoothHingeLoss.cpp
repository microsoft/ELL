////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SmoothHingeLoss.cpp (functions)
//  Authors:  Lin Xiao, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SmoothHingeLoss.h"

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace functions
{
    double SmoothHingeLoss::operator()(double prediction, double label) const
    {
        double margin = prediction * label;
        if (margin >= 1.0)
        {
            return 0.0;
        }
        auto l = 1.0 - margin;
        if (l >= _inverseSmoothness)
        {
            return l - _inverseSmoothness / 2;
        }
        else
        {
            return l*l / (2 * _inverseSmoothness);
        }
    }

    double SmoothHingeLoss::GetDerivative(double prediction, double label) const
    {
        double margin = prediction * label;
        if (margin >= 1.0)
        {
            return 0.0;
        }
        auto l = 1.0 - margin;
        if (l >= _inverseSmoothness)
        {
            return -label;
        }
        else
        {
            return -label * l / _inverseSmoothness;
        }
    }

    double SmoothHingeLoss::Conjugate(double dual, double label) const
    {
        double x = dual * label;
        DEBUG_THROW(x < -1 || x > 0 , utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "dual times label must be in [-1,0]"));
        return x + (_inverseSmoothness / 2) * (x*x);
    }

    double SmoothHingeLoss::ConjugateProx(double sigma, double dual, double label) const
    {
        double b = (dual * label - sigma) / (1.0 + sigma * _inverseSmoothness);
        if (b > 0.0)
        {
            return 0.0;
        }
        if (b < -1.0)
        {
            return -label;
        }
        else
        {
            return b * label;
        }
    }
}
}