////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearPredictorNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictorNode.h"
#include "ConstantNode.h"
#include "DotProductNode.h"
#include "BinaryOperationNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>
#include <cassert>

namespace nodes
{
    LinearPredictorNode::LinearPredictorNode() : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1)
    {
    }

    LinearPredictorNode::LinearPredictorNode(const model::OutputPortElements<double>& input, const predictors::LinearPredictor& predictor) : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
    }

    void LinearPredictorNode::Compute() const
    {
        auto result = _predictor.GetBias();
        const auto weights = _predictor.GetWeights();
        for (size_t index = 0; index < _input.Size(); index++)
        {
            result += _input[index] * weights[index];
        }
        _output.SetOutput({ result });
    }

    void LinearPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<LinearPredictorNode>(newOutputPortElements, _predictor);
        transformer.MapOutputPort(output, newNode->output);
    }

    void LinearPredictorNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newOutputs = BuildSubModel(_predictor, transformer.GetModel(), newOutputPortElements);
        transformer.MapOutputPort(output, newOutputs.output);
    }

    LinearPredictorNodeOutputs BuildSubModel(const predictors::LinearPredictor& predictor, model::Model& model, const model::OutputPortElements<double>& outputPortElements)
    {
        auto weightsNode = model.AddNode<ConstantNode<double>>(predictor.GetWeights());
        auto dotProductNode = model.AddNode<DotProductNode<double>>(weightsNode->output, outputPortElements);
        auto biasNode = model.AddNode<ConstantNode<double>>(predictor.GetBias());
        auto addNode = model.AddNode<BinaryOperationNode<double>>(dotProductNode->output, biasNode->output, BinaryOperationNode<double>::OperationType::add);
        return { addNode->output };
    }

    void LinearPredictorNode::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("input", _input);
        serializer.Serialize("output", _output);
        serializer.Serialize("predictor", _predictor);
    }

    void LinearPredictorNode::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("input", _input, context);
        serializer.Deserialize("output", _output, context);
        serializer.Deserialize("predictor", _predictor, context);
    }
}
