////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TanhActivation.h (neural)
//  Authors:  James Devine
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Activation.h"
// math
#include "Tensor.h"
#include "Vector.h"

// stl
#include <cmath>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Implements the hyperbolic tangent function: tanh(x) = 2 . sigmoid(2x) - 1 </summary>
        template <typename ElementType>
        class TanhActivation : public ActivationImpl<ElementType>
        {
        public:
            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value. </param>
            ElementType Apply(const ElementType input) const override;

            /// <summary> Gets the name of this type. </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("TanhActivation"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Make a copy of this activation. </summary>
            ///
            /// <returns> The copy in a unique pointer. </param>
            std::unique_ptr<ActivationImpl<ElementType>> Copy() const override;
        };
    } // namespace neural
} // namespace predictors
} // namespace ell

#include "../tcc/TanhActivation.tcc"
