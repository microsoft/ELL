////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReLUActivation.h (neural)
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
    /// <summary> Implements the ReLU function: loss(x) = (x > 0) ? x : 0  </summary>
    template <typename ElementType>
    class ReLUActivation
    {
    public:
        /// <summary> Sets the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ElementType Apply(const ElementType input) const;

        /// <summary> Typename used for serialization. </summary>
        static std::string GetTypeName() { return "ReLUActivation"; }
    };
}
}
}

#include "../tcc/ReLUActivation.tcc"