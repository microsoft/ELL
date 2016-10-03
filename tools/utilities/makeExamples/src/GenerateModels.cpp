////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     GenerateModels.cpp (makeExamples)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// common
#include "IsNodeCompilable.h"

// model
#include "InputNode.h"
#include "Model.h"

// nodes
#include "BinaryOperationNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ForestPredictorNode.h"
#include "L2NormNode.h"
#include "LinearPredictorNode.h"
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"

// predictors
#include "ForestPredictor.h"
#include "LinearPredictor.h"
#include "SingleElementThresholdPredictor.h"

// stl
#include <string>

namespace emll
{
model::Model GenerateModel1()
{
    // For now, just create a model and return it
    const int dimension = 3;
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
    auto mean8 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
    auto var8 = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, 8);
    auto mean16 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 16);
    auto var16 = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, 16);

    // classifier
    auto inputs = model::Concat(model::MakePortElements(mean8->output), model::MakePortElements(var8->output), model::MakePortElements(mean16->output), model::MakePortElements(var16->output));
    predictors::LinearPredictor predictor(inputs.Size());
    // Set some values into the predictor's vector
    for (int index = 0; index < inputs.Size(); ++index)
    {
        predictor.GetWeights()[index] = (double)(index % 5);
    }
    auto classifierNode = model.AddNode<nodes::LinearPredictorNode>(inputs, predictor);
    return model;
}

model::Model GenerateModel2()
{
    const int dimension = 3;
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);

    // one "leg"
    auto mean1 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
    auto mag1 = model.AddNode<nodes::L2NormNode<double>>(mean1->output);

    // other "leg"
    auto mag2 = model.AddNode<nodes::L2NormNode<double>>(inputNode->output);
    auto mean2 = model.AddNode<nodes::MovingAverageNode<double>>(mag2->output, 8);

    // combine them
    auto diff = model.AddNode<nodes::BinaryOperationNode<double>>(mag1->output, mean2->output, nodes::BinaryOperationType::subtract);
    return model;
}

model::Model GenerateModel3()
{
    const int dimension = 3;
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
    auto lowpass = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 16);
    auto highpass = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, lowpass->output, nodes::BinaryOperationType::subtract);

    auto delay1 = model.AddNode<nodes::DelayNode<double>>(highpass->output, 4);
    auto delay2 = model.AddNode<nodes::DelayNode<double>>(highpass->output, 8);

    auto dot1 = model.AddNode<nodes::DotProductNode<double>>(highpass->output, delay1->output);
    auto dot2 = model.AddNode<nodes::DotProductNode<double>>(highpass->output, delay2->output);

    auto dotDifference = model.AddNode<nodes::BinaryOperationNode<double>>(dot1->output, dot2->output, nodes::BinaryOperationType::subtract);
    return model;
}

predictors::SimpleForestPredictor CreateForest(size_t numSplits)
{
    // define some abbreviations
    using SplitAction = predictors::SimpleForestPredictor::SplitAction;
    using SplitRule = predictors::SingleElementThresholdPredictor;
    using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;
    using NodeId = predictors::SimpleForestPredictor::SplittableNodeId;

    // build a forest
    predictors::SimpleForestPredictor forest;
    SplitRule dummyRule{ 0, 0.0 };
    EdgePredictorVector dummyEdgePredictor{ -1.0, 1.0 };
    auto root = forest.Split(SplitAction{ forest.GetNewRootId(), dummyRule, dummyEdgePredictor });
    std::vector<size_t> interiorNodeVector;
    interiorNodeVector.push_back(root);

    for (int index = 0; index < numSplits; ++index)
    {
        auto node = interiorNodeVector.back();
        interiorNodeVector.pop_back();
        interiorNodeVector.push_back(forest.Split(SplitAction{ forest.GetChildId(node, 0), dummyRule, dummyEdgePredictor }));
        interiorNodeVector.push_back(forest.Split(SplitAction{ forest.GetChildId(node, 1), dummyRule, dummyEdgePredictor }));
    }
    return forest;
}

model::Model GenerateTreeModel(size_t numSplits)
{
    auto forest = CreateForest(numSplits);
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto simpleForestPredictorNode = model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);
    return model;
}

model::Model GenerateRefinedTreeModel(size_t numSplits)
{
    auto model = GenerateTreeModel(numSplits);
    model::TransformContext context{ common::IsNodeCompilable() };
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    return refinedModel;
}
}