////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LeakyReLUActivation.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "Exception.h"

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
        LeakyReLUActivation(ElementType leakyFactor = static_cast<ElementType>(0.1)) : _leakyFactor(leakyFactor) {}

        /// <summary> Sets the output as a function of the input. </summary>
        ///
        /// <param name="input"> The input value. </param>
        ElementType Apply(const ElementType input) const;

        /// <summary> Gets the leaky factor parameter. </summary>
        ///
        /// <returns> The leaky factor parameter. </returns>
        ElementType GetLeakyFactor() const { return _leakyFactor; }

        /// <summary> Typename used for serialization. </summary>
        static std::string GetTypeName() { return "LeakyReLUActivation"; }

    private:
        ElementType _leakyFactor;
    };
}
}
}

#include "../tcc/LeakyReLUActivation.tcc"