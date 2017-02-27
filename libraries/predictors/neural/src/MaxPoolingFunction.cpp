////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MaxPoolingFunction.cpp (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MaxPoolingFunction.h"

// stl
#include <algorithm>
#include <limits> 

namespace ell
{
namespace predictors
{
namespace neural
{
    MaxPoolingFunction::MaxPoolingFunction() : _max(-(std::numeric_limits<double>::max()))
    {
    }

    void MaxPoolingFunction::Accumulate(double input)
    {
        _max = std::max(_max, input);
    }

    double MaxPoolingFunction::GetValue() const
    {
        return _max;
    }
}
}
}
