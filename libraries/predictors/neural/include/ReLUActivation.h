////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReLUActivation.h (neural)
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
    /// <summary> Implements the ReLU function: loss(x) = (x > 0) ? x : 0  </summary>
    template <typename ElementType>
    class ReLUActivation
    {
    public:
        /// <summary> Sets the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ElementType Apply(const ElementType input) const;
    };
}
}
}

#include "../tcc/ReLUActivation.tcc"