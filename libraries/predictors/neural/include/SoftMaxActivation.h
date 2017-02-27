////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftMaxActivation.h (neural)
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
    /// <summary> Implements the softmax function, which creates output as probabilities which sum up to 1. </summary>
    class SoftMaxActivation
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
