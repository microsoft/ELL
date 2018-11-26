////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReLUActivation.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Activation.h"

#include <math/include/Tensor.h>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Implements the ReLU function: loss(x) = (x > 0) ? x : 0  </summary>
        template <typename ElementType>
        class ReLUActivation : public ActivationImpl<ElementType>
        {
        public:
            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value. </param>
            ///
            /// <returns> The computed output. </param>
            ElementType Apply(const ElementType input) const override;

            /// <summary> Gets the name of this type. </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("ReLUActivation"); }

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

#pragma region implementation

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        ElementType ReLUActivation<ElementType>::Apply(const ElementType input) const
        {
            return ((input > 0) ? input : 0);
        }

        template <typename ElementType>
        std::unique_ptr<ActivationImpl<ElementType>> ReLUActivation<ElementType>::Copy() const
        {
            return std::make_unique<ReLUActivation<ElementType>>();
        }
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
