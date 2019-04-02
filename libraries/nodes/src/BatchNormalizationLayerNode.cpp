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
    BatchNormalizationLayerNode<ValueType>::BatchNormalizationLayerNode(const model::OutputPort<ValueType>& input, const predictors::neural::BatchNormalizationLayer<ValueType>& layer) :
        NeuralNetworkLayerNode<BatchNormalizationLayerNode<ValueType>, predictors::neural::BatchNormalizationLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType>
    bool BatchNormalizationLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(this->input);
        auto scaleValues = this->_layer.GetScale().ToArray();
        auto biasValues = this->_layer.GetBias().ToArray();
        const auto& scaleValuesOut = Constant(transformer, scaleValues);
        const auto& biasValuesOut = Constant(transformer, biasValues);

        const size_t dimension = 2;
        auto computeNode = transformer.AddNode<BroadcastLinearFunctionNode<ValueType>>(newInput,
                                                                                       this->GetInputMemoryLayout(),
                                                                                       scaleValuesOut,
                                                                                       biasValuesOut,
                                                                                       dimension,
                                                                                       this->GetOutputMemoryLayout());

        transformer.MapNodeOutput(this->output, computeNode->output);
        return true;
    }

    template <typename ValueType>
    void BatchNormalizationLayerNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(this->_input);
        auto newNode = transformer.AddNode<BatchNormalizationLayerNode<ValueType>>(newPortElements, this->_layer);
        transformer.MapNodeOutput(this->_output, newNode->output);
    }

    //
    // Explicit specialization of various classes
    //

    template class BatchNormalizationLayerNode<float>;
    template class BatchNormalizationLayerNode<double>;
} // namespace nodes
} // namespace ell
