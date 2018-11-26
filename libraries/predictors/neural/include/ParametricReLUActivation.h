////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParametricReLUActivation.h (neural)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Activation.h"

#include <utilities/include/Unused.h>

#include <math/include/Tensor.h>

namespace ell
{
namespace predictors
{
    namespace neural
    {
        /// <summary> Implements the parametric ReLU function: activation(x, alpha, i) = (x[i] > 0) ? x[i] : alpha[i] * x[i] </summary>
        template <typename ElementType>
        class ParametricReLUActivation : public ActivationImpl<ElementType>
        {
        public:
            using ConstTensorReferenceType = math::ConstTensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
            using TensorType = math::Tensor<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;

            ParametricReLUActivation() = default;

            /// <summary> Instantiates an instance of this class with the learned alpha parameters. </summary>
            ///
            /// <param name="alpha"> The learned alpha parameters. </param>
            ParametricReLUActivation(TensorType alpha);

            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value. </param>
            ///
            /// <returns> The computed output. </param>
            ElementType Apply(const ElementType input) const override;

            /// <summary> Make a copy of this activation. </summary>
            ///
            /// <returns> The copy in a unique pointer. </param>
            std::unique_ptr<ActivationImpl<ElementType>> Copy() const override;

            /// <summary> Returns the output as a function of the input. </summary>
            ///
            /// <param name="input"> The input value. </param>
            /// <param name="index"> The input index. </param>
            ///
            /// <returns> The computed output. </param>
            ElementType ApplyIndex(const ElementType input, const math::IntegerTriplet& index) const override;

            /// <summary> Gets the name of this type. </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("ParametricReLUActivation"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

            /// <summary> Gets the learned alpha parameters. </summary>
            ///
            /// <returns> The learned alpha parameters. </returns>
            const TensorType& GetAlpha() const { return _alpha; }

            /// <summary> Archives this object. </summary>
            ///
            /// <param name="archiver"> The archiver. </param>
            void WriteToArchive(utilities::Archiver& archiver) const override;

            /// <summary> Unarchives this object. </summary>
            ///
            /// <param name="archiver"> The unarchiver. </param>
            void ReadFromArchive(utilities::Unarchiver& archiver) override;

        private:
            TensorType _alpha;
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
        ParametricReLUActivation<ElementType>::ParametricReLUActivation(TensorType alpha) :
            _alpha(std::move(alpha))
        {
            if (_alpha.GetDataPointer() == nullptr)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "alpha tensor has null data field");
            }
        }

        template <typename ElementType>
        ElementType ParametricReLUActivation<ElementType>::Apply(const ElementType input) const
        {
            UNUSED(input);
            // we want people to call the ApplyIndex method in this case.
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        template <typename ElementType>
        ElementType ParametricReLUActivation<ElementType>::ApplyIndex(const ElementType input, const math::IntegerTriplet& index) const
        {
            return ((input > 0) ? input : _alpha(index) * input);
        }

        template <typename ElementType>
        void ParametricReLUActivation<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
        {
            math::TensorArchiver::Write(_alpha, "alpha", archiver);
        }

        template <typename ElementType>
        void ParametricReLUActivation<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
        {
            math::TensorArchiver::Read(_alpha, "alpha", archiver);
        }

        template <typename ElementType>
        std::unique_ptr<ActivationImpl<ElementType>> ParametricReLUActivation<ElementType>::Copy() const
        {
            return std::make_unique<ParametricReLUActivation<ElementType>>(_alpha);
        }
    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
