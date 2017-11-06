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
#include "SquaredEuclideanDistanceNode.h"
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
        : Node({ &_input }, { &_outputScores }), _input(this, {}, inputPortName), _outputScores(this, outputPortName, 0)
    {
    }

    ProtoNNPredictorNode::ProtoNNPredictorNode(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor)
        : Node({ &_input }, { &_outputScores }), _input(this, input, inputPortName), _outputScores(this, outputPortName, predictor.GetNumLabels()), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
    }

    void ProtoNNPredictorNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _outputScores;
        archiver["predictor"] << _predictor;
    }

    void ProtoNNPredictorNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _outputScores;
        archiver["predictor"] >> _predictor;
    }

    void ProtoNNPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ProtoNNPredictorNode>(newPortElements, _predictor);
        transformer.MapNodeOutput(outputScores, newNode->outputScores);
    }

    bool ProtoNNPredictorNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

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

        transformer.MapNodeOutput(outputScores, labelScoresNode->output);

        return true;
    }

    void ProtoNNPredictorNode::Compute() const
    {
        auto inputDataVector = ProtoNNPredictor::DataVectorType(_input.GetIterator());

        auto prediction = _predictor.Predict(inputDataVector);

        _outputScores.SetOutput(prediction.ToArray());
    }

    ProtoNNPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<ProtoNNPredictorNode>(input, predictor);
    }
}
}
