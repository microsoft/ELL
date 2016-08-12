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
    LinearPredictorNode::LinearPredictorNode(const model::OutputPortElements<double>& input, const predictors::LinearPredictor& predictor) : Node({ &_input }, { &_output, &_weightedElements }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _weightedElements(this, weightedElementsPortName, input.Size()), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
    }

    void LinearPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<LinearPredictorNode>(newOutputPortElements, _predictor);
        transformer.MapOutputPort(output, newNode->output);
        transformer.MapOutputPort(weightedElements, newNode->weightedElements);
    }

    void LinearPredictorNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
    
        auto weightsNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetWeights());
        auto dotProductNode = transformer.AddNode<DotProductNode<double>>(weightsNode->output, newOutputPortElements);
        auto coordinatewiseMultiplyNode = transformer.AddNode<BinaryOperationNode<double>>(weightsNode->output, newOutputPortElements, BinaryOperationNode<double>::OperationType::coordinatewiseMultiply);
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

    LinearPredictorNode* AddNodeToModelTransformer(const model::OutputPortElements<double>& input, const predictors::LinearPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<LinearPredictorNode>(input, predictor);
    }
}
