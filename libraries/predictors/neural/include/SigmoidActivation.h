////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SigmoidActivation.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Implements the sigmoid function: loss(x) = 1.0 / (1.0 + exp(-1.0 * x)) </summary>
    template <typename ElementType>
    class SigmoidActivation
    {
    public:
        /// <summary> Sets the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ElementType Apply(const ElementType input) const;

        /// <summary> Typename used for serialization. </summary>
        static std::string GetTypeName() { return "SigmoidActivation"; }
    };
}
}
}

#include "../tcc/SigmoidActivation.tcc"