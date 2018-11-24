////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BiasLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BiasLayerNode.h"
#include "BroadcastFunctionNode.h"
#include "ConstantNode.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    BiasLayerNode<ValueType>::BiasLayerNode(const model::OutputPort<ValueType>& input, const predictors::neural::BiasLayer<ValueType>& layer) :
        NeuralNetworkLayerNode<BiasLayerNode<ValueType>, predictors::neural::BiasLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType>
    bool BiasLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(this->input);
        auto biasValues = this->_layer.GetBias().ToArray();
        auto scaleValuesNode = transformer.AddNode<ConstantNode<ValueType>>(); // nothing
        auto biasValuesNode = transformer.AddNode<ConstantNode<ValueType>>(biasValues);

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

    template <typename ValueType>
    void BiasLayerNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(this->_input);
        auto newNode = transformer.AddNode<BiasLayerNode<ValueType>>(newPortElements, this->_layer);
        transformer.MapNodeOutput(this->_output, newNode->output);
    }

    // Explicit specialization
    template class BiasLayerNode<float>;
    template class BiasLayerNode<double>;
} // namespace nodes
} // namespace ell
