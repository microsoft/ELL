////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Nodes_test.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nodes_test.h"
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"
#include "DelayNode.h"
#include "AccumulatorNode.h"
#include "BinaryOperationNode.h"
#include "UnaryOperationNode.h"
#include "L2NormNode.h"
#include "LinearPredictorNode.h"
#include "ForestNode.h"
#include "DemultiplexerNode.h"

// model
#include "Model.h"
#include "Node.h"
#include "InputNode.h"

// predictors
#include "LinearPredictor.h"

// common
#include "IsNodeCompilable.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

using namespace nodes;

//
// Helpers
//
namespace
{
    double VectorMagnitude(const std::vector<double>& vec)
    {
        double sumSq = 0.0;
        for (const auto& x : vec)
            sumSq += (x * x);

        return std::sqrt(sumSq);
    }

    double VectorMean(const std::vector<double>& vec)
    {
        double sum = 0.0;
        for (const auto& x : vec)
            sum += x;

        return sum / vec.size();
    }

    double VectorVariance(const std::vector<double>& vec, double mean)
    {
        if (vec.size() == 0)
            return 0.0;
        double var = 0.0;
        for (auto x : vec)
        {
            double diff = x - mean;
            var += diff * diff;
        }
        return var / vec.size();
    }

    std::ostream& operator<<(std::ostream& out, const std::vector<double> array)
    {
        out << "[";
        for (auto x : array)
            out << x << "  ";
        out << "]";
        return out;
    }
}

//
// Test compute functions
//

void TestL2NormNodeCompute()
{
    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(data[0].size());
    auto outputNode = model.AddNode<nodes::L2NormNode<double>>(inputNode->output);

    bool okSize = true;
    bool okResult = true;
    for (int index = 0; index < data.size(); ++index)
    {
        auto inputValue = data[index];
        double expectedOutput = VectorMagnitude(inputValue);

        inputNode->SetInput(inputValue);
        std::vector<double> outputVec = model.ComputeOutput(outputNode->output);

        testing::ProcessTest("Testing L2NormNode output size", outputVec.size() == 1);
        testing::ProcessTest("Testing L2NormNode compute", testing::IsEqual(outputVec[0], expectedOutput));
    }
}

void TestAccumulatorNodeCompute()
{
    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(data[0].size());
    auto outputNode = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);

    std::vector<double> accumOutput(data[0].size());

    bool ok = true;
    for (int index = 0; index < data.size(); ++index)
    {
        auto inputValue = data[index];
        for (int d = 0; d < inputValue.size(); d++)
        {
            accumOutput[d] += inputValue[d];
        }
        inputNode->SetInput(inputValue);
        std::vector<double> outputVec = model.ComputeOutput(outputNode->output);

        testing::ProcessTest("Testing AccumulatorNode compute", testing::IsEqual(outputVec, accumOutput));
    }
}

void TestDelayNodeCompute()
{
    const int delay = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto outputNode = model.AddNode<nodes::DelayNode<double>>(inputNode->output, delay);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    std::vector<double> outputVec;

    bool ok = true;
    for (int index = 0; index < data.size(); ++index)
    {
        auto inputValue = data[index];
        inputNode->SetInput(inputValue);
        outputVec = model.ComputeOutput(outputNode->output);
        if (index >= delay)
        {
            testing::ProcessTest("Testing DelayNode compute", testing::IsEqual(outputVec, data[index - delay]));
        }
    }
}

void TestMovingAverageNodeCompute()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto outputNode = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };
    double expectedOutput = VectorMean({ 7.0, 8.0, 9.0, 10.0 });

    std::vector<double> outputVec;

    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        outputVec = model.ComputeOutput(outputNode->output);
    }
    testing::ProcessTest("Testing MovingAverageNode compute", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestMovingVarianceNodeCompute()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto outputNode = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };
    double mean = VectorMean({ 7.0, 8.0, 9.0, 10.0 });
    double expectedOutput = VectorVariance({ 7.0, 8.0, 9.0, 10.0 }, mean);

    std::vector<double> outputVec;

    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        outputVec = model.ComputeOutput(outputNode->output);
    }
    testing::ProcessTest("Testing MovingVarianceNode compute", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestUnaryOperationNodeCompute()
{
    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(data[0].size());
    auto outputNode = model.AddNode<nodes::UnaryOperationNode<double>>(inputNode->output, nodes::UnaryOperationNode<double>::OperationType::sqrt);

    for (int index = 0; index < data.size(); ++index)
    {
        auto inputValue = data[index];

        inputNode->SetInput(inputValue);
        std::vector<double> outputVec = model.ComputeOutput(outputNode->output);

        for (int d = 0; d < inputValue.size(); ++d)
        {
            auto expectedOutput = std::sqrt(inputValue[d]);
            testing::ProcessTest("Testing UnaryOperationNode compute", testing::IsEqual(outputVec[d], expectedOutput));
        }
    }
}

void TestBinaryOperationNodeCompute()
{
    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(data[0].size());
    auto outputNode = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, inputNode->output, nodes::BinaryOperationNode<double>::OperationType::add);

    for (int index = 0; index < data.size(); ++index)
    {
        auto inputValue = data[index];

        inputNode->SetInput(inputValue);
        std::vector<double> outputVec = model.ComputeOutput(outputNode->output);

        for (int d = 0; d < inputValue.size(); ++d)
        {
            auto expectedOutput = 2 * inputValue[d];
            testing::ProcessTest("Testing BinaryOperationNode compute", testing::IsEqual(outputVec[d], expectedOutput));
        }
    }
}

void TestLinearPredictorNodeCompute()
{
    const int dim = 10;
    predictors::LinearPredictor predictor(dim);
    // TODO: set its vector and bias

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dim);
    auto predNode = model.AddNode<nodes::LinearPredictorNode>(inputNode->output, predictor);

    // TODO: test it
}

void TestDemultiplexerNodeCompute()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto selectorNode = model.AddNode<model::InputNode<bool>>(1);
    auto muxNode = model.AddNode<nodes::DemultiplexerNode<double, bool>>(inputNode->output, selectorNode->output, 2);

    std::vector<double> inputValue{ 5.0 };
    inputNode->SetInput(inputValue);

    selectorNode->SetInput(false); // output[0] should get the input
    auto outputVec = model.ComputeOutput(muxNode->output);
    testing::ProcessTest("Testing DemultiplexerNode compute", testing::IsEqual(outputVec, {5.0, 0}));

    selectorNode->SetInput(true); // output[1] should get the input
    outputVec = model.ComputeOutput(muxNode->output);
    testing::ProcessTest("Testing DemultiplexerNode compute", testing::IsEqual(outputVec, {0.0, 5.0}));
}

//
// Node refinements
//

// TODO: make a generic TestModelsEqual function that takes 2 models, 2 output nodes, and a test message string

void TestMovingAverageNodeRefine()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto meanNode = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };
    double expectedOutput = VectorMean({ 7.0, 8.0, 9.0, 10.0 });

    model::TransformContext context{ common::IsNodeCompilable() };
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    auto refinedInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto refinedOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ meanNode->output }); // TODO: cleanup

    std::cout << "MovingAverage model compilable: " << (transformer.IsModelCompilable() ? "yes" : "no") << std::endl;
    std::cout << "Original nodes: " << model.Size() << ", refined: " << refinedModel.Size() << std::endl;

    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        auto outputVec1 = model.ComputeOutput(meanNode->output);
        refinedInputNode->SetInput(inputValue);
        auto outputVec2 = refinedModel.ComputeOutput(refinedOutputElements);

        testing::ProcessTest("Testing MovingAverageNode refine", testing::IsEqual(outputVec1, outputVec2));
    }
}

void TestSimpleForestNodeRefine()
{
    // define some abbreviations
    using SplitAction = predictors::SimpleForestPredictor::SplitAction;
    using SplitRule = predictors::SingleElementThresholdPredictor;
    using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;
    using NodeId = predictors::SimpleForestPredictor::SplittableNodeId;

    // build a forest
    predictors::SimpleForestPredictor forest;
    auto root = forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.3 }, EdgePredictorVector{ -1.0, 1.0 } });
    forest.Split(SplitAction{ forest.GetChildId(root, 0), SplitRule{ 1, 0.6 }, EdgePredictorVector{ -2.0, 2.0 } });
    forest.Split(SplitAction{ forest.GetChildId(root, 1), SplitRule{ 2, 0.9 }, EdgePredictorVector{ -4.0, 4.0 } });
    forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.2 }, EdgePredictorVector{ -3.0, 3.0 } });

    // build the model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto simpleForestNode = model.AddNode<nodes::SimpleForestNode>(inputNode->output, forest);

    // refine
    model::TransformContext context{ common::IsNodeCompilable() };
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    auto refinedInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto refinedOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ simpleForestNode->output });
    auto refinedTreeOutputsElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ simpleForestNode->treeOutputs });
    auto refinedEdgeIndicatorVectorElements = transformer.GetCorrespondingOutputs(model::PortElements<bool>{ simpleForestNode->edgeIndicatorVector });
    testing::ProcessTest("Testing SimpleForestNode compilable", testing::IsEqual(transformer.IsModelCompilable(), true));

    // check equivalence
    inputNode->SetInput({ 0.2, 0.5, 0.0 });
    refinedInputNode->SetInput({ 0.2, 0.5, 0.0 });
    auto outputValue = model.ComputeOutput(simpleForestNode->output)[0];
    auto treeOutputsValue = model.ComputeOutput(simpleForestNode->treeOutputs);
    auto edgeIndicatorVectorValue = model.ComputeOutput(simpleForestNode->edgeIndicatorVector);

    auto refinedOutputValue = refinedModel.ComputeOutput(refinedOutputElements)[0];
    auto refinedTreeOutputsValue = refinedModel.ComputeOutput(refinedTreeOutputsElements);
    auto refinedEdgeIndicatorVectorValue = refinedModel.ComputeOutput(refinedEdgeIndicatorVectorElements);

    //  expected output is -3.0
    testing::ProcessTest("Testing SimpleForestNode refine (output)", testing::IsEqual(outputValue, refinedOutputValue));
    testing::ProcessTest("Testing SimpleForestNode refine (treeOutputs)", testing::IsEqual(treeOutputsValue, refinedTreeOutputsValue));
    testing::ProcessTest("Testing SimpleForestNode refine (edgeIndicatorVector)", testing::IsEqual(edgeIndicatorVectorValue, refinedEdgeIndicatorVectorValue));
}

void TestLinearPredictorNodeRefine()
{
    // make a linear predictor
    size_t dim = 3;
    predictors::LinearPredictor predictor(dim);
    predictor.GetBias() = 2.0;
    predictor.GetWeights() = std::vector<double>{ 3.0, 4.0, 5.0 };

    // make a model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto linearPredictorNode = model.AddNode<nodes::LinearPredictorNode>(inputNode->output, predictor);

    // refine the model
    model::TransformContext context{ common::IsNodeCompilable() };
    model::ModelTransformer transformer;
    auto newModel = transformer.RefineModel(model, context);
    testing::ProcessTest("Testing LinearPredictorNode compilable", testing::IsEqual(transformer.IsModelCompilable(), true));

    // check for equality
    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto newOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ linearPredictorNode->output }); // TODO: cleanup
    inputNode->SetInput({ 1.0, 1.0, 1.0 });
    newInputNode->SetInput({ 1.0, 1.0, 1.0 });
    auto modelOutputValue = model.ComputeOutput(linearPredictorNode->output)[0];
    auto newOutputValue = newModel.ComputeOutput(newOutputElements)[0];

    testing::ProcessTest("Testing LinearPredictorNode refine", testing::IsEqual(modelOutputValue, newOutputValue));
}

void TestDemultiplexerNodeRefine()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto selectorNode = model.AddNode<model::InputNode<bool>>(1);
    auto muxNode = model.AddNode<nodes::DemultiplexerNode<double, bool>>(inputNode->output, selectorNode->output, 2);

    // refine the model
    model::TransformContext context{ common::IsNodeCompilable() };
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    testing::ProcessTest("Testing DemultiplexerNode compilable", testing::IsEqual(transformer.IsModelCompilable(), true));

    std::cout << "Demultiplexer model compilable: " << (transformer.IsModelCompilable() ? "yes" : "no") << std::endl;
    std::cout << "Original nodes: " << model.Size() << ", refined: " << refinedModel.Size() << std::endl;

    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto newSelectorNode = transformer.GetCorrespondingInputNode(selectorNode);
    auto newMuxNodeElements = transformer.GetCorrespondingOutputs(muxNode->output); 

    std::vector<double> inputValue{ 5.0 };
    inputNode->SetInput(inputValue);
    newInputNode->SetInput(inputValue);

    selectorNode->SetInput(false);
    newSelectorNode->SetInput(false); // output[0] should get the input
    auto outputVec = model.ComputeOutput(muxNode->output);
    auto newOutputVec = refinedModel.ComputeOutput(newMuxNodeElements);
    testing::ProcessTest("Testing DemultiplexerNode refine", testing::IsEqual(outputVec, newOutputVec));

    selectorNode->SetInput(true);
    newSelectorNode->SetInput(true); // output[1] should get the input
    outputVec = model.ComputeOutput(muxNode->output);
    newOutputVec = refinedModel.ComputeOutput(newMuxNodeElements);
    testing::ProcessTest("Testing DemultiplexerNode refine", testing::IsEqual(outputVec, newOutputVec));
}
