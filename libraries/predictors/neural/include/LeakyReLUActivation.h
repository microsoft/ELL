////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LeakyReLUActivation.h (neural)
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
    /// <summary> Implements the leaky ReLU function: activation(x) = (x > 0) ? x : leakyFactor * x  </summary>
    class LeakyReLUActivation
    {
    public:
        /// <summary> Instantiates an instance of this class with appropriate leaky factor. 
        /// Typical values are 0.1 and 0.01.
        /// </summary>
        ///
        /// <param name="leakyFactor"> The value to multiply the input by if it is less than zero. </param>
        LeakyReLUActivation(double leakyFactor = 0.1) : _leakyFactor(leakyFactor) {}

        /// <summary> Sets the values of the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input values. </param>
        /// <param name="output"> The output values. </param>
        void Apply(const LayerVector& input, LayerVector& output) const;
    private:
        double _leakyFactor;
    };
}
}
}
