////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorNode.cpp (nodes)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NeuralNetworkPredictorNode.h"
#include "ConstantNode.h"

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
    template <typename ValueType>
    NeuralNetworkPredictorNode<ValueType>::NeuralNetworkPredictorNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    NeuralNetworkPredictorNode<ValueType>::NeuralNetworkPredictorNode(const model::PortElements<ValueType>& input, const Predictor& predictor)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, predictor.NumOutputs()), _predictor(predictor)
    {
        assert(input.Size() == _predictor.NumInputs());
    }

    template <typename ValueType>
    NeuralNetworkPredictorNode<ValueType>::NeuralNetworkPredictorNode(const model::PortElements<ValueType>& input, Predictor&& predictor)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, predictor.NumOutputs()), _predictor(std::move(predictor))
    {
        assert(input.Size() == _predictor.NumInputs());
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
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        //TODO
        //auto newInputElements = transformer.TransformPortElements(_input.GetPortElements());

        //size_t prevOutputSize = _input.Size();
        //auto layerInputs = newInputElements;
        //Node* lastNode = nullptr;
        //for (const auto& layer : _predictor.GetLayers())
        //{
        //    auto numInputs = layer->NumInputs();
        //    assert(prevOutputSize == layer->NumInputs());
        //    auto layerNode = AddLayerNode(transformer, *layer, layerInputs);
        //    prevOutputSize = layer->NumOutputs();
        //    layerInputs = model::PortElements<ValueType>{ *layerNode->GetOutputPort(0) };
        //    lastNode = layerNode;
        //}

        //transformer.MapNodeOutput(_output, *lastNode->GetOutputPort(0));
        //return true;
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        //TODO
        //auto inputDataVector = Predictor::DataVectorType(_input.GetIterator());
        //_output.SetOutput({ ConvertVectorTo<ValueType>(_predictor.Predict(inputDataVector)) });
    }

    // explicit specialization for float, double
    template class NeuralNetworkPredictorNode<float>;
    template class NeuralNetworkPredictorNode<double>;

}
}
