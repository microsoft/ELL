////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorNode.cpp (nodes)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NeuralNetworkPredictorNode.h"
#include "ConstantNode.h"
#include "ReorderDataNode.h"

// data
#include "DenseDataVector.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    namespace
    {
        // Handy helper function
        size_t GetShapeSize(const math::Triplet& shape)
        {
            return shape[0] * shape[1] * shape[2];
        }
    }

    template <typename ValueType>
    NeuralNetworkPredictorNode<ValueType>::NeuralNetworkPredictorNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    NeuralNetworkPredictorNode<ValueType>::NeuralNetworkPredictorNode(const model::PortElements<ValueType>& input, const PredictorType& predictor)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, GetShapeSize(predictor.GetOutputShape())), _predictor(predictor)
    {
        assert(input.Size() == GetShapeSize(_predictor.GetInputShape()));
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver["predictor"] << _predictor;
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        PredictorType::RegisterNeuralNetworkPredictorTypes(archiver.GetContext());
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver["predictor"] >> _predictor;
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInputElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<NeuralNetworkPredictorNode>(newInputElements, _predictor);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool NeuralNetworkPredictorNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInputElements = transformer.TransformPortElements(_input.GetPortElements());

        const auto& inputLayer = _predictor.GetInputLayer();
        auto inputShape = inputLayer.GetInputShape();
        auto outputPadding = inputLayer.GetLayerParameters().outputPaddingParameters;
        auto padding = outputPadding.paddingSize;

        DataShape inputNodeInputShape({ inputShape[0], inputShape[1], inputShape[2] });
        DataShape inputNodeOutputShape({ inputShape[0] + padding, inputShape[1] + padding, inputShape[2] }, { padding, padding, 0 });
        auto padedInputNode = transformer.AddNode<ReorderDataNode<ValueType>>(newInputElements, inputNodeInputShape, inputNodeOutputShape);

        size_t prevOutputSize = GetShapeSize(inputLayer.GetOutputShape());
        auto layerInputs = model::PortElements<ValueType>(padedInputNode->output);
        Node* lastNode = nullptr;
        for (const auto& layer : _predictor.GetLayers())
        {
            auto numInputs = GetShapeSize(layer->GetInputShape());
            assert(prevOutputSize == GetShapeSize(layer->GetInputShape()));
            auto layerNode = AddLayerNode(transformer, *layer, layerInputs);
            prevOutputSize = GetShapeSize(layer->GetOutputShape());
            layerInputs = model::PortElements<ValueType>{ *layerNode->GetOutputPort(0) };
            lastNode = layerNode;
        }

        transformer.MapNodeOutput(_output, *lastNode->GetOutputPort(0));
        return true;
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::Compute() const
    {
        auto inputDataVector = typename PredictorType::DataVectorType(_input.GetIterator());
        _output.SetOutput({ _predictor.Predict(inputDataVector) });
    }

    // explicit specialization for float, double
    template class NeuralNetworkPredictorNode<float>;
    template class NeuralNetworkPredictorNode<double>;
}
}
