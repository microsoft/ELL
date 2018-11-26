////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftmaxLayer.h (neural)
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
        /// <summary> A layer in a neural network that provides a softmax mapping of the input to output such that the output probabilities sum up to 1. </summary>
        template <typename ElementType>
        class SoftmaxLayer : public Layer<ElementType>
        {
        public:
            using LayerParameters = typename Layer<ElementType>::LayerParameters;
            using Layer<ElementType>::GetOutputMinusPadding;
            using Layer<ElementType>::AssignValues;

            /// <summary> Instantiates an instance of a softmax layer. </summary>
            ///
            /// <param name="layerParameters"> The parameters common to every layer. </param>
            SoftmaxLayer(const LayerParameters& layerParameters);

            /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
            SoftmaxLayer() {}

            /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
            void Compute() override;

            /// <summary> Indicates the kind of layer. </summary>
            ///
            /// <returns> An enum indicating the layer type. </returns>
            LayerType GetLayerType() const override { return LayerType::softmax; }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("SoftmaxLayer"); }

            /// <summary> Gets the name of this type (for serialization). </summary>
            ///
            /// <returns> The name of this type. </returns>
            std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        private:
            using Layer<ElementType>::_layerParameters;
            using Layer<ElementType>::_output;
        };

    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma region implementation

#include <limits>

namespace ell
{
namespace predictors
{
    namespace neural
    {

        template <typename ElementType>
        SoftmaxLayer<ElementType>::SoftmaxLayer(const LayerParameters& layerParameters) :
            Layer<ElementType>(layerParameters)
        {
            if (_layerParameters.input.Size() != GetOutputMinusPadding().Size())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, GetRuntimeTypeName() + ": Expected size of input and output tensor (minus padding) to match");
            }
        }

        template <typename ElementType>
        void SoftmaxLayer<ElementType>::Compute()
        {
            auto output = GetOutputMinusPadding();
            auto& input = _layerParameters.input;

            AssignValues(input, output);

            ElementType sum = 0;
            ElementType maxValue = std::numeric_limits<ElementType>::lowest();

            // Find the max
            for (size_t i = 0; i < input.NumRows(); i++)
            {
                for (size_t j = 0; j < input.NumColumns(); j++)
                {
                    for (size_t k = 0; k < input.NumChannels(); k++)
                    {
                        ElementType value = input(i, j, k);
                        maxValue = std::max(maxValue, value);
                    }
                }
            }

            // Use the max to calculate the Euler value
            for (size_t i = 0; i < input.NumRows(); i++)
            {
                for (size_t j = 0; j < input.NumColumns(); j++)
                {
                    for (size_t k = 0; k < input.NumChannels(); k++)
                    {
                        ElementType value = input(i, j, k);
                        ElementType eulerVal = std::exp(value - maxValue);
                        output(i, j, k) = eulerVal;
                        sum += eulerVal;
                    }
                }
            }

            // Divide the value by the sum. After this, the sum of all values will be 1.0
            for (size_t i = 0; i < input.NumRows(); i++)
            {
                for (size_t j = 0; j < input.NumColumns(); j++)
                {
                    for (size_t k = 0; k < input.NumChannels(); k++)
                    {
                        output(i, j, k) /= sum;
                    }
                }
            }
        }

    } // namespace neural
} // namespace predictors
} // namespace ell

#pragma endregion implementation
