////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleElementThresholdNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SingleElementThresholdNode.h"
#include "ConstantNode.h"
#include "BinaryPredicateNode.h"

// dataset
#include "DenseDataVector.h"

namespace nodes
{
    SingleElementThresholdNode::SingleElementThresholdNode(const model::OutputPortElements<double>& input, const predictors::SingleElementThresholdPredictor& predictor) : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _predictor(predictor)
    {
        assert(input.Size() > predictor.GetElementIndex());
    }

    void SingleElementThresholdNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<SingleElementThresholdNode>(newOutputPortElements, _predictor);
        transformer.MapOutputPort(output, newNode->output);
    }

    void SingleElementThresholdNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());

        // get the element used in the split rule 
        // TODO - waiting for OutputPortElements changes: the following 3 lines should be one line. 
        auto elementAsRange = newOutputPortElements.GetElement(_predictor.GetElementIndex());
        model::OutputPortElements<double> element;
        element.AddRange(elementAsRange);

        // get the threshold.
        auto thresholdNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetThreshold());

        // create a predicate that implements the split rule
        auto binaryPredicateNode = transformer.AddNode<BinaryPredicateNode<double>>(element, thresholdNode->output, BinaryPredicateNode<double>::PredicateType::greater);

        transformer.MapOutputPort(output, binaryPredicateNode->output);
    }

    void SingleElementThresholdNode::Compute() const
    {
        // create an IDataVector
        dataset::DoubleDataVector dataVector(_input.GetValue());

        // predict
        _output.SetOutput({ _predictor.Predict(dataVector) });
    }

    SingleElementThresholdNode* AddNodeToModelTransformer(const model::OutputPortElements<double>& input, const predictors::SingleElementThresholdPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<SingleElementThresholdNode>(input, predictor);
    }


}