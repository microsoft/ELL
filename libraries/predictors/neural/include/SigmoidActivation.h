////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SigmoidActivation.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ILayer.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    using LayerVector = predictors::neural::ILayer::LayerVector;
    /// <summary> Implements the sigmoid function: loss(x) = 1.0 / (1.0 + exp(-1.0 * x)) </summary>
    class SigmoidActivation
    {
    public:
        /// <summary> Sets the values of the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input values. </param>
        /// <param name="output"> The output values. </param>
        void Apply(const LayerVector& input, LayerVector& output) const;
    };
}
}
}
