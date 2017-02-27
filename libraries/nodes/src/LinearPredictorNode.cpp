////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearPredictorNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictorNode.h"
#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "DotProductNode.h"

// utilities
#include "Exception.h"

// data
#include "DenseDataVector.h"

// stl
#include <cassert>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    LinearPredictorNode::LinearPredictorNode()
        : Node({ &_input }, { &_output, &_weightedElements }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _weightedElements(this, weightedElementsPortName, 0)
    {
    }

    LinearPredictorNode::LinearPredictorNode(const model::PortElements<double>& input, const predictors::LinearPredictor& predictor)
        : Node({ &_input }, { &_output, &_weightedElements }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _weightedElements(this, weightedElementsPortName, input.Size()), _predictor(predictor)
    {
        assert(input.Size() == predictor.Size());
    }

    void LinearPredictorNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver["weightedElements"] << _weightedElements;
        archiver["predictor"] << _predictor;
    }

    void LinearPredictorNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver["weightedElements"] >> _weightedElements;
        archiver["predictor"] >> _predictor;
    }

    void LinearPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<LinearPredictorNode>(newPortElements, _predictor);
        transformer.MapNodeOutput(output, newNode->output);
        transformer.MapNodeOutput(weightedElements, newNode->weightedElements);
    }

    bool LinearPredictorNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

        auto weightsNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetWeights().ToArray());
        auto dotProductNode = transformer.AddNode<DotProductNode<double>>(weightsNode->output, newPortElements);
        auto coordinatewiseMultiplyNode = transformer.AddNode<BinaryOperationNode<double>>(weightsNode->output, newPortElements, emitters::BinaryOperationType::coordinatewiseMultiply);
        auto biasNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetBias());
        auto addNode = transformer.AddNode<BinaryOperationNode<double>>(dotProductNode->output, biasNode->output, emitters::BinaryOperationType::add);

        transformer.MapNodeOutput(output, addNode->output);
        transformer.MapNodeOutput(weightedElements, coordinatewiseMultiplyNode->output);
        return true;
    }

    void LinearPredictorNode::Compute() const
    {
        auto inputDataVector = LinearPredictor::DataVectorType(_input.GetIterator());
        _output.SetOutput({ _predictor.Predict(inputDataVector) });
        _weightedElements.SetOutput(_predictor.GetWeightedElements(inputDataVector).ToArray());
    }

    LinearPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::LinearPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<LinearPredictorNode>(input, predictor);
    }
}
}
