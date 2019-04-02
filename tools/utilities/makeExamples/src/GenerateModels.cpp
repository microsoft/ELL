////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GenerateModels.cpp (makeExamples)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GenerateModels.h"

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>
#include <model/include/RefineTransformation.h>
#include <model/include/SliceNode.h>
#include <model/include/SpliceNode.h>

#include <nodes/include/ActivationLayerNode.h>
#include <nodes/include/BiasLayerNode.h>
#include <nodes/include/BinaryOperationNode.h>
#include <nodes/include/BroadcastFunctionNode.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/DelayNode.h>
#include <nodes/include/DotProductNode.h>
#include <nodes/include/ExtremalValueNode.h>
#include <nodes/include/ForestPredictorNode.h>
#include <nodes/include/FullyConnectedLayerNode.h>
#include <nodes/include/L2NormSquaredNode.h>
#include <nodes/include/LinearPredictorNode.h>
#include <nodes/include/MovingAverageNode.h>
#include <nodes/include/MovingVarianceNode.h>

#include <predictors/include/ForestPredictor.h>
#include <predictors/include/LinearPredictor.h>
#include <predictors/include/NeuralNetworkPredictor.h>
#include <predictors/include/SingleElementThresholdPredictor.h>

#include <predictors/neural/include/BiasLayer.h>
#include <predictors/neural/include/FullyConnectedLayer.h>
#include <predictors/neural/include/Layer.h>
#include <predictors/neural/include/ReLUActivation.h>

#include <sstream>
#include <string>

using namespace ell;
using namespace model;
using namespace nodes;
using namespace predictors;
using namespace predictors::neural;

model::Model GenerateIdentityModel(size_t dimension)
{
    model::Model model;
    const auto& input = model::Input<double>(model, dimension);
    model::Output(input);
    return model;
}

model::Model GenerateTimesTwoModel(size_t dimension)
{
    model::Model model;
    const auto& input = model::Input<double>(model, dimension);
    const auto& constantTwo = nodes::Constant(model, std::vector<double>(dimension, 2.0));
    const auto& product = nodes::Multiply(input, constantTwo);
    model::Output(product);
    return model;
}

template <typename ElementType>
model::Model GenerateBroadcastTimesTwoModel(size_t dimension)
{
    model::Model model;
    const auto& input = model::Input<ElementType>(model, dimension);
    const auto& constantTwo = nodes::Constant(model, std::vector<ElementType>(1, 2.0));
    const auto& null = nodes::Constant(model, std::vector<ElementType>{});
    model::PortMemoryLayout layout({ static_cast<int>(dimension), 1 });
    const auto& product = nodes::BroadcastLinearFunction(input, layout, constantTwo, null, 1, layout);
    model::Output(product);
    return model;
}

model::Model GenerateIsEqualModel()
{
    model::Model model;
    const auto& input = model::Input<double>(model, 2);
    const auto& elem0 = model::Slice(input, 0, 1);
    const auto& elem1 = model::Slice(input, 1, 1);
    const auto& predicate = nodes::Equal(elem0, elem1);
    model::Output(predicate);
    return model;
}

model::Model GenerateArgMaxModel(size_t dimension)
{
    model::Model model;
    const auto& input = model::Input<double>(model, dimension);
    model::Output(nodes::ArgMax(input).argVal);
    return model;
}

model::Model GenerateMultiOutModel(size_t dimension)
{
    model::Model model;
    const auto& input = model::Input<double>(model, dimension);
    const auto& constantTwo = nodes::Constant(model, std::vector<double>(dimension, 2.0));
    const auto& constantTen = nodes::Constant(model, std::vector<double>(dimension, 10.0));
    const auto& product = nodes::Multiply(input, constantTwo);
    const auto& sum = nodes::Add(input, constantTen);
    model::Output(model::Splice(product, sum));
    return model;
}

model::Model GenerateModel1()
{
    // For now, just create a model and return it
    const int dimension = 3;
    model::Model model;
    const auto& input = model::Input<double>(model, dimension);
    const auto& mean8 = nodes::MovingAverage(input, 8);
    const auto& var8 = nodes::MovingVariance(input, 8);
    const auto& mean16 = nodes::MovingAverage(input, 16);
    const auto& var16 = nodes::MovingVariance(input, 16);

    // classifier
    const auto& inputs = model::Splice(mean8, var8, mean16, var16);
    predictors::LinearPredictor<double> predictor(inputs.Size());
    // Set some values into the predictor's vector
    for (size_t index = 0; index < inputs.Size(); ++index)
    {
        predictor.GetWeights()[index] = (double)(index % 5);
    }
    [[maybe_unused]] const auto& predictorOutput = nodes::LinearPredictor(inputs, predictor);
    return model;
}

model::Model GenerateModel2()
{
    const int dimension = 3;
    model::Model model;
    const auto& input = model::Input<double>(model, dimension);

    // one "leg"
    const auto& mag1 = nodes::L2NormSquared(nodes::MovingAverage(input, 8));

    // other "leg"
    const auto& mean2 = nodes::MovingAverage(nodes::L2NormSquared(input), 8);

    // combine them
    [[maybe_unused]] const auto& combination = nodes::Subtract(mag1, mean2);
    return model;
}

model::Model GenerateModel3()
{
    const int dimension = 3;
    model::Model model;
    const auto& input = model::Input<double>(model, dimension);
    const auto& lowpass = nodes::MovingAverage(input, 16);
    const auto& highpass = nodes::Subtract(input, lowpass);

    const auto& dot1 = nodes::DotProduct(highpass, nodes::Delay(highpass, 4));
    const auto& dot2 = nodes::DotProduct(highpass, nodes::Delay(highpass, 8));

    [[maybe_unused]] const auto& result = nodes::Subtract(dot1, dot2);
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
    const auto& input = model::Input<double>(model, 3);
    [[maybe_unused]] const auto& predictorOutput = nodes::ForestPredictor(input, forest);
    return model;
}

model::Model GenerateRefinedTreeModel(size_t numSplits)
{
    auto model = GenerateTreeModel(numSplits);
    model::TransformContext context;
    model::ModelTransformer transformer;
    model::RefineTransformation t;
    return t.TransformModel(model, transformer, context);
}

// explicit instantiations
template model::Model GenerateBroadcastTimesTwoModel<int>(size_t dimension);

template model::Model GenerateBroadcastTimesTwoModel<float>(size_t dimension);

template model::Model GenerateBroadcastTimesTwoModel<double>(size_t dimension);
