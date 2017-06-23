////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BatchNormalizationLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BatchNormalizationLayerNode.h"
#include "BroadcastFunctionNode.h"
#include "ConstantNode.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    BatchNormalizationLayerNode<ValueType>::BatchNormalizationLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::BatchNormalizationLayer<ValueType>& layer)
        : NeuralNetworkLayerNode<BatchNormalizationLayerNode<ValueType>, predictors::neural::BatchNormalizationLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType>
    bool BatchNormalizationLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& layerParameters = this->_layer.GetLayerParameters();

        // Calculate input dimension parameters
        const size_t inputPaddingSize = layerParameters.inputPaddingParameters.paddingSize;
        auto inputShape = this->_layer.GetInputShape();
        std::vector<size_t> inputStride{ inputShape.begin(), inputShape.end() };
        std::vector<size_t> inputOffset = { inputPaddingSize, inputPaddingSize, 0 };
        std::vector<size_t> inputSize = inputStride;
        for (int dimensionIndex = 0; dimensionIndex < inputOffset.size(); ++dimensionIndex)
        {
            inputSize[dimensionIndex] -= 2 * inputOffset[dimensionIndex];
        }

        const size_t outputPaddingSize = layerParameters.outputPaddingParameters.paddingSize;
        auto outputShape = this->_layer.GetOutputShape();
        std::vector<size_t> outputStride{ outputShape.begin(), outputShape.end() };
        std::vector<size_t> outputOffset = { outputPaddingSize, outputPaddingSize, 0 };

        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());
        auto scaleValues = this->_layer.GetScale().ToArray();
        auto scaleValuesNode = transformer.AddNode<ConstantNode<ValueType>>(scaleValues);
        auto biasValues = this->_layer.GetBias().ToArray();
        auto biasValuesNode = transformer.AddNode<ConstantNode<ValueType>>(biasValues);

        const size_t dimension = 2;
        auto computeNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(newInput,
                                                                                       inputStride,
                                                                                       inputOffset,
                                                                                       inputSize,
                                                                                       scaleValuesNode->output,
                                                                                       biasValuesNode->output,
                                                                                       dimension,
                                                                                       outputStride,
                                                                                       outputOffset);

        transformer.MapNodeOutput(this->output, computeNode->output);
        return true;
    }

    //
    // Explicit specialization of various classes
    //

    template class BatchNormalizationLayerNode<float>;
    template class BatchNormalizationLayerNode<double>;
} // nodes
} // ell
