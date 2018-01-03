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
#include "L2NormSquaredNode.h"
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
        : Node({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    ProtoNNPredictorNode::ProtoNNPredictorNode(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor)
        : Node({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, predictor.GetNumLabels()), _predictor(predictor)
    {
        assert(input.Size() == predictor.GetDimension());
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

        auto outNode = transformer.AddNode<model::OutputNode<double>>(labelScoresNode->output);

        transformer.MapNodeOutput(output, outNode->output);

        return true;
    }

    void ProtoNNPredictorNode::Compute() const
    {
        auto indexValueIterator = _input.GetIterator();

        // densify the index/value pairs directly into a std::vector, which avoids making a copy via DoubleDataVector.
        std::vector<double> inputData;
        while (indexValueIterator.IsValid())
        {
            auto current = indexValueIterator.Get();
            auto index = current.index;
            double value = current.value;
            inputData.resize(index + 1);
            inputData.back() = value;
            indexValueIterator.Next();
        }

        auto prediction = _predictor.Predict(inputData);

        _output.SetOutput(prediction.ToArray());
    }

    ProtoNNPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<ProtoNNPredictorNode>(input, predictor);
    }
}
}
