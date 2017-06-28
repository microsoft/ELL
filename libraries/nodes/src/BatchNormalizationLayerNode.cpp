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
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());
        auto scaleValues = this->_layer.GetScale().ToArray();
        auto scaleValuesNode = transformer.AddNode<ConstantNode<ValueType>>(scaleValues);
        auto biasValues = this->_layer.GetBias().ToArray();
        auto biasValuesNode = transformer.AddNode<ConstantNode<ValueType>>(biasValues);

        const size_t dimension = 2;
        auto computeNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(newInput,
                                                                                       this->GetInputMemoryLayout(),
                                                                                       scaleValuesNode->output,
                                                                                       biasValuesNode->output,
                                                                                       dimension,
                                                                                       this->GetOutputMemoryLayout());

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
