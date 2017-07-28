////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReLUActivation.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Tensor.h"

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
        /// <param name="index"> The input index. </param>
        ///
        /// <returns> The computed output. </param>
        ElementType Apply(const ElementType input, const math::Triplet& index) const;

        /// <summary> Typename used for serialization. </summary>
        static std::string GetTypeName() { return "ReLUActivation"; }
    };
}
}
}

#include "../tcc/ReLUActivation.tcc"