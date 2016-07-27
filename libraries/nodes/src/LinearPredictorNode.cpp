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
    LinearPredictorNode::LinearPredictorNode(const model::OutputPortElements<double>& input, const predictors::LinearPredictor& predictor) : Node({ &_input }, { &_prediction, &_weightedElements }), _input(this, input, inputPortName), _prediction(this, outputPortName, 1), _weightedElements(this, weightedElementsPortName, input.Size()), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
    }

    void LinearPredictorNode::Compute() const
    {
        // create an IDataVector
        dataset::DoubleDataVector dataVector; 
        for (size_t i = 0; i < _input.Size(); ++i)
        {
            dataVector.AppendEntry(i, _input[i]);
        }
        
        // predict
        _prediction.SetOutput({ _predictor.Predict(dataVector) });
        _weightedElements.SetOutput(_predictor.GetWeightedElements(dataVector));
    }

    void LinearPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<LinearPredictorNode>(newOutputPortElements, _predictor);
        transformer.MapOutputPort(prediction, newNode->prediction);
        transformer.MapOutputPort(weightedElements, newNode->weightedElements);
    }

    void LinearPredictorNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newOutputs = BuildSubModel(_predictor, transformer.GetModel(), newOutputPortElements);
        transformer.MapOutputPort(prediction, newOutputs.prediction);
        transformer.MapOutputPort(weightedElements, newOutputs.weightedElements);
    }

    LinearPredictorNodeOutputs BuildSubModel(const predictors::LinearPredictor& predictor, model::Model& model, const model::OutputPortElements<double>& outputPortElements)
    {
        auto weightsNode = model.AddNode<ConstantNode<double>>(predictor.GetWeights());
        auto dotProductNode = model.AddNode<DotProductNode<double>>(weightsNode->output, outputPortElements);
        auto coordinatewiseMultiplyNode = model.AddNode<BinaryOperationNode<double>>(weightsNode->output, outputPortElements, BinaryOperationNode<double>::OperationType::coordinatewiseMultiply);
        auto biasNode = model.AddNode<ConstantNode<double>>(predictor.GetBias());
        auto addNode = model.AddNode<BinaryOperationNode<double>>(dotProductNode->output, biasNode->output, BinaryOperationNode<double>::OperationType::add);
        return { addNode->output, coordinatewiseMultiplyNode->output };
    }
}
