////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReLUActivation.cpp (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ReLUActivation.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    void ReLUActivation::Apply(const LayerVector& input, LayerVector& output) const
    {
        if (input.Size() > output.Size())
        {
            throw std::invalid_argument("Expected output vector to have at least as many elements as the input vector");
        }

        for (size_t i = 0; i < input.Size(); i++)
        {
            output[i] = (input[i] > 0) ? input[i] : 0;
        }        
    }
}
}
}
