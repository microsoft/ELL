////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FullyConnectedLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FullyConnectedLayerNode.h"
#include "BroadcastFunctionNode.h"
#include "ConstantNode.h"
#include "MatrixVectorMultiplyNode.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace nodes
{
    template<typename ValueType>
    FullyConnectedLayerNode<ValueType>::FullyConnectedLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::FullyConnectedLayer<ValueType>& layer)
        : NeuralNetworkLayerNode<FullyConnectedLayerNode<ValueType>, predictors::neural::FullyConnectedLayer<ValueType>, ValueType>(input, layer)
    {
        const auto& layerParameters = layer.GetLayerParameters();
        if (HasPadding(layerParameters.inputPaddingParameters))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "FullyConnectedLayerNode does not currently support inputs with padding");
        }

        if (HasPadding(layerParameters.outputPaddingParameters))
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "FullyConnectedLayerNode does not currently support outputs with padding");
        }
    }

    template<typename ValueType>
    bool FullyConnectedLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& layerParameters = this->GetLayer().GetLayerParameters();

        // Calculate input dimension parameters
        size_t inputPaddingSize = layerParameters.inputPaddingParameters.paddingSize;
        auto inputShape = this->_layer.GetInputShape();
        std::vector<size_t> inputStride = inputShape;
        std::vector<size_t> inputOffset = { inputPaddingSize, inputPaddingSize, 0 };
        std::vector<size_t> inputSize = inputStride;
        for (size_t dimensionIndex = 0; dimensionIndex < inputOffset.size(); ++dimensionIndex)
        {
            inputSize[dimensionIndex] -= 2 * inputOffset[dimensionIndex];
        }

        size_t outputPaddingSize = layerParameters.outputPaddingParameters.paddingSize;
        auto outputShape = this->_layer.GetOutputShape();
        std::vector<size_t> outputStride = outputShape;
        std::vector<size_t> outputOffset = { outputPaddingSize, outputPaddingSize, 0 };

        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // TODO: add a reorder node here that makes the input be a contiguous vector, if necessary

        const auto& weights = this->_layer.GetWeights();
        auto m = weights.NumRows();
        auto n = weights.NumColumns();
        auto lda = weights.GetIncrement();
        auto weightsValues = weights.ToArray();
        auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);
        auto matrixMultiplyNode = transformer.AddNode<MatrixVectorMultiplyNode<ValueType>>(weightsNode->output, m, n, lda, newInput);

        // TODO: add a reorder node here that adds padding to the output, if necessary

        transformer.MapNodeOutput(this->output, matrixMultiplyNode->output);
        return true;
    }

    // Explicit specialization
    template class FullyConnectedLayerNode<float>;
    template class FullyConnectedLayerNode<double>;
} // nodes
} // ell
