////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProjectionPredictorNode.cpp (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProtoNNPredictorNode.h"

#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "L2NormSquaredNode.h"
#include "MatrixVectorProductNode.h"
#include "SquaredEuclideanDistanceNode.h"
#include "UnaryOperationNode.h"

#include <utilities/include/Exception.h>

#include <data/include/DenseDataVector.h>

#include <functional>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    ProtoNNPredictorNode::ProtoNNPredictorNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {
    }

    ProtoNNPredictorNode::ProtoNNPredictorNode(const model::OutputPort<double>& input, const predictors::ProtoNNPredictor& predictor) :
        Node({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, predictor.GetNumLabels()),
        _predictor(predictor)
    {
        if (input.Size() != predictor.GetDimension())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "ProtoNNPredictorNode: input size must match the predictor dimension");
        }
    }

    void ProtoNNPredictorNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[defaultOutputPortName] << _output;
        archiver["predictor"] << _predictor;
    }

    void ProtoNNPredictorNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[defaultOutputPortName] >> _output;
        archiver["predictor"] >> _predictor;
    }

    void ProtoNNPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<ProtoNNPredictorNode>(newPortElements, _predictor);
        transformer.MapNodeOutput(output, newNode->output);
    }

    bool ProtoNNPredictorNode::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);

        // Projection
        auto projectionMatrix = _predictor.GetProjectionMatrix();
        auto projecedInputNode = transformer.AddNode<MatrixVectorProductNode<double, math::MatrixLayout::columnMajor>>(newPortElements, projectionMatrix);

        auto prototypes = _predictor.GetPrototypes();
        auto m = _predictor.GetNumPrototypes();

        std::vector<double> multiplier(m, _predictor.GetGamma() * _predictor.GetGamma() * -1);
        auto gammaNode = transformer.AddNode<ConstantNode<double>>(multiplier);

        // Distance to each prototype
        math::RowMatrixReference<double> prototypesMatrix = prototypes.Transpose();
        auto squareDistanceNode = transformer.AddNode<SquaredEuclideanDistanceNode<double, math::MatrixLayout::rowMajor>>(projecedInputNode->output, prototypesMatrix);

        // Similarity to each prototype
        auto scaledDistanceNode = transformer.AddNode<BinaryOperationNode<double>>(squareDistanceNode->output, gammaNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);
        auto expDistanceNode = transformer.AddNode<UnaryOperationNode<double>>(scaledDistanceNode->output, emitters::UnaryOperationType::exp);

        // Get the prediction label
        auto labelScoresNode = transformer.AddNode<MatrixVectorProductNode<double, math::MatrixLayout::columnMajor>>(expDistanceNode->output, _predictor.GetLabelEmbeddings());

        transformer.MapNodeOutput(output, labelScoresNode->output);

        return true;
    }

    void ProtoNNPredictorNode::Compute() const
    {
        auto data = _input.GetValue();
        std::vector<double> inputData;
        inputData.reserve(data.size());
        std::transform(data.begin(), data.end(), std::back_inserter(inputData), [](auto x) { return static_cast<double>(x); });
        auto prediction = _predictor.Predict(inputData);

        _output.SetOutput(prediction.ToArray());
    }

    ProtoNNPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<ProtoNNPredictorNode>(input, predictor);
    }
} // namespace nodes
} // namespace ell
