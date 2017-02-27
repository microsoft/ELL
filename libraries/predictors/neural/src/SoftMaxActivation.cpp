////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftMaxActivation.cpp (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SoftMaxActivation.h"

// STL
#include <limits>

namespace ell
{
namespace predictors
{
namespace neural
{
    void SoftMaxActivation::Apply(const LayerVector& input, LayerVector& output) const
    {
        if (input.Size() > output.Size())
        {
            throw std::invalid_argument("Expected output vector to have at least as many elements as the input vector");
        }

        double sum = 0;
        double maxVal = -std::numeric_limits<double>::max();
        for (size_t i = 0; i < input.Size(); i++)
        {
            maxVal = std::max(maxVal, input[i]);
        }
        for (size_t i = 0; i < input.Size(); i++)
        {
            double eulerVal = std::exp(input[i] - maxVal);
            output[i] = eulerVal;
            sum += eulerVal;
        }
        for (size_t i = 0; i < input.Size(); i++)
        {
            output[i] /= sum;
        }
    }
}
}
}
