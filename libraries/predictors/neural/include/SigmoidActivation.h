////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SigmoidActivation.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Activation.h"

#include <math/include/Tensor.h>

#include <nodes/include/ActivationFunctions.h>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Implements the sigmoid function: loss(x) = 1.0 / (1.0 + exp(-1.0 * x)) </summary>
        template <typename ElementType>
        class SigmoidActivation : public ActivationImpl<ElementType>
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
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("SigmoidActivation"); }

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

#include <cmath>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        template <typename ElementType>
        ElementType SigmoidActivation<ElementType>::Apply(const ElementType input) const
        {
            ell::nodes::SigmoidActivationFunction<ElementType> sigmoid;
            return sigmoid.Compute(input);
        }

        template <typename ElementType>
        std::unique_ptr<ActivationImpl<ElementType>> SigmoidActivation<ElementType>::Copy() const
        {
            return std::make_unique<SigmoidActivation<ElementType>>();
        }
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
