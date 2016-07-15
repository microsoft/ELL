////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearPredictorNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictorNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>
#include <cassert>

namespace nodes
{
    LinearPredictorNode::LinearPredictorNode(const model::OutputPortElementList<double>& input, const predictors::LinearPredictor& predictor) : Node({ &_input }, { &_output }), _input(this, input, "input"), _output(this, "output", 1), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
    }

    void LinearPredictorNode::Compute() const
    {
        auto result = _predictor.GetBias();
        const auto weights = _predictor.GetVector();
        for (size_t index = 0; index < _input.Size(); index++)
        {
            result += _input[index] * weights[index];
        }
        _output.SetOutput({ result });
    }

    void LinearPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<LinearPredictorNode>(newInput, _predictor);
        transformer.MapOutputPort(output, newNode->output);
    }
}
