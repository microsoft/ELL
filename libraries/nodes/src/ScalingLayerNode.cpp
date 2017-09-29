////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalingLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ScalingLayerNode.h"
#include "BroadcastFunctionNode.h"
#include "ConstantNode.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    ScalingLayerNode<ValueType>::ScalingLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::ScalingLayer<ValueType>& layer)
        : NeuralNetworkLayerNode<ScalingLayerNode<ValueType>, predictors::neural::ScalingLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType>
    bool ScalingLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());
        auto scaleValues = this->_layer.GetScale().ToArray();
        auto scaleValuesNode = transformer.AddNode<ConstantNode<ValueType>>(scaleValues);
        auto biasValuesNode = transformer.AddNode<ConstantNode<ValueType>>(); // nothing

        const size_t channelDimension = 2;
        auto computeNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(newInput,
                                                                                       this->GetInputMemoryLayout(),
                                                                                       scaleValuesNode->output,
                                                                                       biasValuesNode->output,
                                                                                       channelDimension,
                                                                                       this->GetOutputMemoryLayout());
        transformer.MapNodeOutput(this->output, computeNode->output);
        return true;
    }

    // Explicit specialization
    template class ScalingLayerNode<float>;
    template class ScalingLayerNode<double>;
} // nodes
} // ell
