////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProjectionPredictorNode.cpp (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProtoNNPredictorNode.h"

// nodes
#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "DotProductNode.h"
#include "L2NormNode.h"
#include "UnaryOperationNode.h"
#include "MatrixVectorProductNode.h"
#include "EuclideanDistanceNode.h"
#include "ExtremalValueNode.h"

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
    ProtoNNPredictorNode::ProtoNNPredictorNode()
        : Node({ &_input }, { &_outputScore, &_outputLabel }), _input(this, {}, inputPortName), _outputScore(this, outputScorePortName, 0), _outputLabel(this, outputLabelPortName, 1)
    {
    }

    ProtoNNPredictorNode::ProtoNNPredictorNode(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor)
        : Node({ &_input }, { &_outputScore, &_outputLabel }), _input(this, input, inputPortName), _outputScore(this, outputScorePortName, 1), _outputLabel(this, outputLabelPortName, 1), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
    }

    void ProtoNNPredictorNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputScorePortName] << _outputScore;
        archiver[outputLabelPortName] << _outputLabel;
        archiver["predictor"] << _predictor;
    }

    void ProtoNNPredictorNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputScorePortName] >> _outputScore;
        archiver[outputLabelPortName] >> _outputLabel;
        archiver["predictor"] >> _predictor;
    }

    void ProtoNNPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ProtoNNPredictorNode>(newPortElements, _predictor);
        transformer.MapNodeOutput(outputLabel, newNode->outputLabel);
        transformer.MapNodeOutput(outputScore, newNode->outputScore);
    }

    bool ProtoNNPredictorNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

        // Projection
        auto projectionMatrix = _predictor.GetProjectionMatrix();
        auto projecedInputNode = transformer.AddNode<MatrixVectorProductNode<double, math::MatrixLayout::columnMajor>>(newPortElements, projectionMatrix);

        auto prototypes = _predictor.GetPrototypes();
        auto m = _predictor.GetNumPrototypes();
        auto k = _predictor.GetProjectedDimension();

        std::vector<double> multiplier(m, _predictor.GetGamma() * _predictor.GetGamma() * -1);
        auto gammaNode = transformer.AddNode<ConstantNode<double>>(multiplier);

        // Distance to each prototype
        auto distanceNode = transformer.AddNode<EuclideanDistanceNode<double, math::MatrixLayout::rowMajor>>(projecedInputNode->output, prototypes.Transpose());

        // Similarity to each prototype
        auto squareDistanceNode = transformer.AddNode<BinaryOperationNode<double>>(distanceNode->output, distanceNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);
        auto scaledDistanceNode = transformer.AddNode<BinaryOperationNode<double>>(squareDistanceNode->output, gammaNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);
        auto expDistanceNode = transformer.AddNode<UnaryOperationNode<double>>(scaledDistanceNode->output, emitters::UnaryOperationType::exp);

        // Get the prediction label
        auto labelScoresNode = transformer.AddNode<MatrixVectorProductNode<double, math::MatrixLayout::columnMajor>>(expDistanceNode->output, _predictor.GetLabelEmbeddings());
        auto predictionLabelNode = transformer.AddNode<ArgMaxNode<double>>(labelScoresNode->output);

        transformer.MapNodeOutput(outputScore, predictionLabelNode->val);
        transformer.MapNodeOutput(outputLabel, predictionLabelNode->argVal);

        return true;
    }

    void ProtoNNPredictorNode::Compute() const
    {
        auto inputDataVector = ProtoNNPredictor::DataVectorType(_input.GetIterator());

        predictors::ProtoNNPrediction prediction = _predictor.Predict(inputDataVector);

        _outputScore.SetOutput({ prediction.score });
        _outputLabel.SetOutput({ (int)prediction.label });
    }

    ProtoNNPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<ProtoNNPredictorNode>(input, predictor);
    }
}
}
