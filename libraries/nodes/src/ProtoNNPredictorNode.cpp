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
        : Node({ &_input }, { &_output}), _input(this, {}, inputPortName), _output(this, outputPortName, 1)
    {
    }

    ProtoNNPredictorNode::ProtoNNPredictorNode(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor)
        : Node({ &_input }, { &_output}), _input(this, input, inputPortName), _output(this, outputPortName, 1), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
    }

    void ProtoNNPredictorNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
        archiver["predictor"] << _predictor;
    }

    void ProtoNNPredictorNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
        archiver["predictor"] >> _predictor;
    }

    void ProtoNNPredictorNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ProtoNNPredictorNode>(newPortElements, _predictor);
        transformer.MapNodeOutput(output, newNode->output);
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
        auto gammaNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetGamma() * _predictor.GetGamma() * -1);
        model::PortElements<double> similarityOutputs;

        // Similarity to each prototype
        for (size_t i = 0; i < m; ++i)
        {
            std::vector<double> prototype(prototypes.GetColumn(i).ToArray());
            auto prototypeNode = transformer.AddNode<ConstantNode<double>>(prototype);

            auto similarityDistanceNode1 = transformer.AddNode<BinaryOperationNode<double>>(projecedInputNode->output, prototypeNode->output, emitters::BinaryOperationType::subtract);
            auto similarityDistanceNode2 = transformer.AddNode<L2NormNode<double>>(similarityDistanceNode1->output);
            auto squareSimilarityDistNode = transformer.AddNode<BinaryOperationNode<double>>(similarityDistanceNode2->output, similarityDistanceNode2->output, emitters::BinaryOperationType::coordinatewiseMultiply);
            auto scaledSimilarityDistNode = transformer.AddNode<BinaryOperationNode<double>>(squareSimilarityDistNode->output, gammaNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);
            auto similarityMultiplier = transformer.AddNode<UnaryOperationNode<double>>(scaledSimilarityDistNode->output, emitters::UnaryOperationType::exp);
            similarityOutputs.Append(similarityMultiplier->output);
        }

        // Get the prediction label
        auto labelScoresNode = transformer.AddNode<MatrixVectorProductNode<double, math::MatrixLayout::columnMajor>>(similarityOutputs, _predictor.GetLabelEmbeddings());
        auto predictionLabelNode = transformer.AddNode<ArgMaxNode<double>>(labelScoresNode->output);

        transformer.MapNodeOutput(output, predictionLabelNode->argVal);

        return true;
    }

    void ProtoNNPredictorNode::Compute() const
    {
        auto inputDataVector = ProtoNNPredictor::DataVectorType(_input.GetIterator());

        _output.SetOutput({ (int)_predictor.Predict(inputDataVector) });
    }

    ProtoNNPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<ProtoNNPredictorNode>(input, predictor);
    }
}
}
