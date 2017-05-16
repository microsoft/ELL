////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LeakyReLUActivation.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Implements the leaky ReLU function: activation(x) = (x > 0) ? x : leakyFactor * x  </summary>
    template <typename ElementType>
    class LeakyReLUActivation
    {
    public:
        /// <summary> Instantiates an instance of this class with appropriate leaky factor. 
        /// Typical values are 0.1 and 0.01.
        /// </summary>
        ///
        /// <param name="leakyFactor"> The value to multiply the input by if it is less than zero. </param>
        LeakyReLUActivation(ElementType leakyFactor = static_cast<ElementType>(0.01)) : _leakyFactor(leakyFactor) {}

        /// <summary> Sets the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ElementType Apply(const ElementType input) const;

    private:
        ElementType _leakyFactor;
    };
}
}
}

#include "../tcc/LeakyReLUActivation.tcc"