////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SingleElementThresholdNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SingleElementThresholdNode.h"
#include "BinaryPredicateNode.h"
#include "ConstantNode.h"

// data
#include "DenseDataVector.h"

namespace ell
{
namespace nodes
{
    SingleElementThresholdNode::SingleElementThresholdNode()
        : Node({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 1)
    {
    }

    SingleElementThresholdNode::SingleElementThresholdNode(const model::PortElements<double>& input, const SingleElementThresholdPredictor& predictor)
        : Node({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, 1), _predictor(predictor)
    {
        assert(input.Size() > predictor.GetElementIndex());
    }

    void SingleElementThresholdNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["predictor"] << _predictor;
    }

    void SingleElementThresholdNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["predictor"] >> _predictor;
    }

    void SingleElementThresholdNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SingleElementThresholdNode>(newPortElements, _predictor);
        transformer.MapNodeOutput(output, newNode->output);
    }

    bool SingleElementThresholdNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

        // get the element used in the split rule
        model::PortElements<double> element{ newPortElements, _predictor.GetElementIndex() };

        // get the threshold.
        auto thresholdNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetThreshold());

        // create a predicate that implements the split rule
        auto binaryPredicateNode = transformer.AddNode<BinaryPredicateNode<double>>(element, thresholdNode->output, emitters::BinaryPredicateType::greater);

        transformer.MapNodeOutput(output, binaryPredicateNode->output);
        return true;
    }

    void SingleElementThresholdNode::Compute() const
    {
        auto inputDataVector = SingleElementThresholdPredictor::DataVectorType(_input.GetIterator());
        _output.SetOutput({ _predictor.Predict(inputDataVector) });
    }

    SingleElementThresholdNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::SingleElementThresholdPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<SingleElementThresholdNode>(input, predictor);
    }
}
}
