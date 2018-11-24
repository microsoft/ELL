////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HardSigmoidActivation.h (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Activation.h"

// math
#include "Tensor.h"

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Implements the "hard sigmoid" function: loss(x) = clip(0.2x + 0.5, 0, 1) (where clip(y, a,b) == max(min(y,b),a)) </summary>
        template <typename ElementType>
        class HardSigmoidActivation : public ActivationImpl<ElementType>
        {
        public:
            /// <summary> Applies the activation function to a value. </summary>
            ///
            /// <param name="input"> The input value. </param>
            ///
            /// <returns> The computed output. </param>
            ElementType Apply(const ElementType input) const override;

            /// <summary> Gets the name of this type. </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("HardSigmoidActivation"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Make a copy of this activation. </summary>
            ///
            /// <returns> The copy in a unique pointer. </param>
            std::unique_ptr<ActivationImpl<ElementType>> Copy() const override;
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#include "../tcc/HardSigmoidActivation.tcc"
