////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GenerateModels.cpp (makeExamples)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// model
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// nodes
#include "BinaryOperationNode.h"
#include "BroadcastFunctionNode.h"
#include "ConstantNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "ForestPredictorNode.h"
#include "L2NormSquaredNode.h"
#include "LinearPredictorNode.h"
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"

// predictors
#include "ForestPredictor.h"
#include "LinearPredictor.h"
#include "SingleElementThresholdPredictor.h"

// stl
#include <sstream>
#include <string>

namespace ell
{
model::Model GenerateIdentityModel(size_t dimension)
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
    model.AddNode<model::OutputNode<double>>(inputNode->output);
    return model;
}

model::Model GenerateTimesTwoModel(size_t dimension)
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
    auto constantTwoNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>(dimension, 2.0));
    auto timesNode = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, constantTwoNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);
    model.AddNode<model::OutputNode<double>>(timesNode->output);
    return model;
}

template <typename ElementType>
model::Model GenerateBroadcastTimesTwoModel(size_t dimension)
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(dimension);
    auto constantTwoNode = model.AddNode<nodes::ConstantNode<ElementType>>(std::vector<ElementType>(1, 2.0));
    model::PortMemoryLayout layout({static_cast<int>(dimension), 1});
    auto timesNode = model.AddNode<nodes::BroadcastLinearFunctionNode<ElementType>>(inputNode->output, layout, constantTwoNode->output, model::PortElements<ElementType>{}, 1, layout);
    model.AddNode<model::OutputNode<ElementType>>(timesNode->output);
    return model;
}

model::Model GenerateIsEqualModel()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(2);
    auto predicateNode = model.AddNode<nodes::BinaryPredicateNode<double>>(model::PortElements<double>{ inputNode->output, 0 }, model::PortElements<double>{ inputNode->output, 1 }, emitters::BinaryPredicateType::equal);
    model.AddNode<model::OutputNode<bool>>(predicateNode->output);
    return model;
}

model::Model GenerateArgMaxModel(size_t dimension)
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
    auto argMaxNode = model.AddNode<nodes::ArgMaxNode<double>>(inputNode->output);
    model.AddNode<model::OutputNode<int>>(argMaxNode->argVal);
    return model;
}

model::Model GenerateMultiOutModel(size_t dimension)
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
    auto constantTwoNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>(dimension, 2.0));
    auto constantTenNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>(dimension, 10.0));
    auto timesNode = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, constantTwoNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);
    auto plusNode = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, constantTenNode->output, emitters::BinaryOperationType::add);

    model.AddNode<model::OutputNode<double>>(model::PortElements<double>{ { timesNode->output }, { plusNode->output } });
    return model;
}

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
    predictors::LinearPredictor<double> predictor(inputs.Size());
    // Set some values into the predictor's vector
    for (size_t index = 0; index < inputs.Size(); ++index)
    {
        predictor.GetWeights()[index] = (double)(index % 5);
    }
    model.AddNode<nodes::LinearPredictorNode<double>>(inputs, predictor);
    return model;
}

model::Model GenerateModel2()
{
    const int dimension = 3;
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);

    // one "leg"
    auto mean1 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
    auto mag1 = model.AddNode<nodes::L2NormSquaredNode<double>>(mean1->output);

    // other "leg"
    auto mag2 = model.AddNode<nodes::L2NormSquaredNode<double>>(inputNode->output);
    auto mean2 = model.AddNode<nodes::MovingAverageNode<double>>(mag2->output, 8);

    // combine them
    model.AddNode<nodes::BinaryOperationNode<double>>(mag1->output, mean2->output, emitters::BinaryOperationType::subtract);
    return model;
}

model::Model GenerateModel3()
{
    const int dimension = 3;
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
    auto lowpass = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 16);
    auto highpass = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, lowpass->output, emitters::BinaryOperationType::subtract);

    auto delay1 = model.AddNode<nodes::DelayNode<double>>(highpass->output, 4);
    auto delay2 = model.AddNode<nodes::DelayNode<double>>(highpass->output, 8);

    auto dot1 = model.AddNode<nodes::DotProductNode<double>>(highpass->output, delay1->output);
    auto dot2 = model.AddNode<nodes::DotProductNode<double>>(highpass->output, delay2->output);

    model.AddNode<nodes::BinaryOperationNode<double>>(dot1->output, dot2->output, emitters::BinaryOperationType::subtract);
    return model;
}

predictors::SimpleForestPredictor CreateForest(size_t numSplits)
{
    // define some abbreviations
    using SplitAction = predictors::SimpleForestPredictor::SplitAction;
    using SplitRule = predictors::SingleElementThresholdPredictor;
    using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;

    // build a forest
    predictors::SimpleForestPredictor forest;
    SplitRule dummyRule{ 0, 0.0 };
    EdgePredictorVector dummyEdgePredictor{ -1.0, 1.0 };
    auto root = forest.Split(SplitAction{ forest.GetNewRootId(), dummyRule, dummyEdgePredictor });
    std::vector<size_t> interiorNodeVector;
    interiorNodeVector.push_back(root);

    for (size_t index = 0; index < numSplits; ++index)
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
    model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);
    return model;
}

model::Model GenerateRefinedTreeModel(size_t numSplits)
{
    auto model = GenerateTreeModel(numSplits);
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    return refinedModel;
}

// explicit instantiations
template 
model::Model GenerateBroadcastTimesTwoModel<int>(size_t dimension);

template 
model::Model GenerateBroadcastTimesTwoModel<float>(size_t dimension);

template 
model::Model GenerateBroadcastTimesTwoModel<double>(size_t dimension);
}
