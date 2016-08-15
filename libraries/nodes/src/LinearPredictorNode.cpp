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

// dataset
#include "DenseDataVector.h"

// stl
#include <string>
#include <vector>
#include <cassert>

namespace nodes
{
    LinearPredictorNode::LinearPredictorNode() : Node({ &_input }, { &_output, &_weightedElements }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _weightedElements(this, weightedElementsPortName, 0)
    {
    }

    LinearPredictorNode::LinearPredictorNode(const model::PortElements<double>& input, const predictors::LinearPredictor& predictor) : Node({ &_input }, { &_output, &_weightedElements }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _weightedElements(this, weightedElementsPortName, input.Size()), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
    }

    void LinearPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<LinearPredictorNode>(newPortElements, _predictor);
        transformer.MapOutputPort(output, newNode->output);
        transformer.MapOutputPort(weightedElements, newNode->weightedElements);
    }

    void LinearPredictorNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
    
        auto weightsNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetWeights());
        auto dotProductNode = transformer.AddNode<DotProductNode<double>>(weightsNode->output, newPortElements);
        auto coordinatewiseMultiplyNode = transformer.AddNode<BinaryOperationNode<double>>(weightsNode->output, newPortElements, BinaryOperationNode<double>::OperationType::coordinatewiseMultiply);
        auto biasNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetBias());
        auto addNode = transformer.AddNode<BinaryOperationNode<double>>(dotProductNode->output, biasNode->output, BinaryOperationNode<double>::OperationType::add);
        
        transformer.MapOutputPort(output, addNode->output);
        transformer.MapOutputPort(weightedElements, coordinatewiseMultiplyNode->output);
    }

    void LinearPredictorNode::Compute() const
    {
        // create an IDataVector
        dataset::DoubleDataVector dataVector(_input.GetValue());

        // predict
        _output.SetOutput({ _predictor.Predict(dataVector) });
        _weightedElements.SetOutput(_predictor.GetWeightedElements(dataVector));
    }

    LinearPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::LinearPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<LinearPredictorNode>(input, predictor);
    }

    void LinearPredictorNode::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("input", _input);
        serializer.Serialize("output", _output);
        serializer.Serialize("weightedElements", _weightedElements);
        serializer.Serialize("predictor", _predictor);
    }

    void LinearPredictorNode::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("input", _input, context);
        serializer.Deserialize("output", _output, context);
        serializer.Deserialize("weightedElements", _weightedElements, context);
        serializer.Deserialize("predictor", _predictor, context);
    }
}
