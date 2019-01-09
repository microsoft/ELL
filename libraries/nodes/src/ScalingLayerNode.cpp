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
    ScalingLayerNode<ValueType>::ScalingLayerNode(const model::OutputPort<ValueType>& input, const predictors::neural::ScalingLayer<ValueType>& layer) :
        NeuralNetworkLayerNode<ScalingLayerNode<ValueType>, predictors::neural::ScalingLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType>
    bool ScalingLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(this->input);
        auto scaleValues = this->_layer.GetScale().ToArray();
        auto biasValuesNode = transformer.AddNode<ConstantNode<ValueType>>(); // nothing
        const auto& scaleValuesOut = AppendConstant(transformer, scaleValues);
        const auto& biasValuesOut = biasValuesNode->output;

        const size_t channelDimension = 2;
        auto computeNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(newInput,
                                                                                       this->GetInputMemoryLayout(),
                                                                                       scaleValuesOut,
                                                                                       biasValuesOut,
                                                                                       channelDimension,
                                                                                       this->GetOutputMemoryLayout());
        transformer.MapNodeOutput(this->output, computeNode->output);
        return true;
    }

    template <typename ValueType>
    void ScalingLayerNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(this->_input);
        auto newNode = transformer.AddNode<ScalingLayerNode<ValueType>>(newPortElements, this->_layer);
        transformer.MapNodeOutput(this->_output, newNode->output);
    }

    // Explicit specialization
    template class ScalingLayerNode<float>;
    template class ScalingLayerNode<double>;
} // namespace nodes
} // namespace ell
