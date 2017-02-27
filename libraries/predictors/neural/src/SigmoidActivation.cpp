////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SigmoidActivation.cpp (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SigmoidActivation.h"

// math
#include <cmath>

namespace ell
{
namespace predictors
{
namespace neural
{
    void SigmoidActivation::Apply(const LayerVector& input, LayerVector& output) const
    {
        if (input.Size() > output.Size())
        {
            throw std::invalid_argument("Expected output vector to have at least as many elements as the input vector");
        }

        for (size_t i = 0; i < input.Size(); i++)
        {
            if (input[i] >= 0.0)
            {
                double exp_value = std::exp(-input[i]);
                output[i] = 1.0 / (1.0 + exp_value);
            }
            else
            {
                double exp_value = std::exp(input[i]);
                output[i] = exp_value / (1.0 + exp_value);
            }
        }
    }
}
}
}
