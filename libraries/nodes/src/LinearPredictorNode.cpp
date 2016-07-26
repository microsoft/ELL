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
        auto newOutputs = BuildSubModel(transformer.GetModel(), newOutputPortElements, _predictor);
        transformer.MapOutputPort(output, newOutputs.output);
    }

    // TODO: the return type of this function should be a struct of named OutputPortElements 
    LinearPredictorNodeOutputs BuildSubModel(model::Model& model, const model::OutputPortElements<double>& outputPortElements, const predictors::LinearPredictor& predictor)
    {
        auto weightsNode = model.AddNode<ConstantNode<double>>(predictor.GetWeights());
        auto dotProductNode = model.AddNode<DotProductNode<double>>(weightsNode->output, outputPortElements);
        auto biasNode = model.AddNode<ConstantNode<double>>(predictor.GetBias());
        auto addNode = model.AddNode<BinaryOperationNode<double>>(dotProductNode->output, biasNode->output, BinaryOperationNode<double>::OperationType::add);
        return {addNode->output};
    }
}
