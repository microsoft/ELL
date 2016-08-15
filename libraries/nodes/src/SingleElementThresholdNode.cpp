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
    SingleElementThresholdNode::SingleElementThresholdNode(const model::PortElements<double>& input, const predictors::SingleElementThresholdPredictor& predictor) : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _predictor(predictor)
    {
        assert(input.Size() > predictor.GetElementIndex());
    }

    void SingleElementThresholdNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SingleElementThresholdNode>(newPortElements, _predictor);
        transformer.MapOutputPort(output, newNode->output);
    }

    void SingleElementThresholdNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

        // get the element used in the split rule 
        // TODO - waiting for PortElements changes: the following 3 lines should be one line. 
        auto elementAsRange = newPortElements.GetElement(_predictor.GetElementIndex());
        model::PortElements<double> element;
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
        _output.SetOutput({ _predictor.Predict(dataVector) == 1 ? true : false });
    }

    SingleElementThresholdNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::SingleElementThresholdPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<SingleElementThresholdNode>(input, predictor);
    }


}