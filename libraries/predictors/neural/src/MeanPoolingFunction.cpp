////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MeanPoolingFunction.cpp (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MeanPoolingFunction.h"

// math
#include <cmath>

namespace ell
{
namespace predictors
{
namespace neural
{
    MeanPoolingFunction::MeanPoolingFunction() : _sum(0), _numValues(0)
    {
    }

    void MeanPoolingFunction::Accumulate(double input)
    {
        _sum += input;
        _numValues++;
    }

    double MeanPoolingFunction::GetValue() const
    {
        return (_sum / (double)_numValues);
    }
}
}
}
