////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Nodes_test.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nodes_test.h"
#include "DTWPrototype.h"

// nodes
#include "AccumulatorNode.h"
#include "BatchNormalizationLayerNode.h"
#include "BiasLayerNode.h"
#include "BinaryOperationNode.h"
#include "DTWDistanceNode.h"
#include "DelayNode.h"
#include "DemultiplexerNode.h"
#include "ForestPredictorNode.h"
#include "L2NormNode.h"
#include "LinearPredictorNode.h"
#include "MatrixVectorProductNode.h"
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"
#include "NeuralNetworkLayerNode.h"
#include "NeuralNetworkPredictorNode.h"
#include "ProtoNNPredictorNode.h"
#include "SinkNode.h"
#include "SourceNode.h"
#include "UnaryOperationNode.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "Node.h"

// predictors
#include "LinearPredictor.h"
#include "NeuralNetworkPredictor.h"

// predictors/neural
#include "BatchNormalizationLayer.h"
#include "BiasLayer.h"
#include "InputLayer.h"
#include "ProtoNNPredictor.h"

// testing
#include "testing.h"

// stl
#include <chrono>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace ell;
using namespace nodes;

//
// Helpers
//
namespace
{
size_t GetShapeSize(const math::Triplet& shape)
{
    return shape[0] * shape[1] * shape[2];
}

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
    auto outputNode = model.AddNode<nodes::UnaryOperationNode<double>>(inputNode->output, emitters::UnaryOperationType::sqrt);

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

void TestUnaryOperationNodeCompute1()
{
    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(data[0].size());
    auto outputNode = model.AddNode<nodes::UnaryOperationNode<double>>(inputNode->output, emitters::UnaryOperationType::exp);
    for (int index = 0; index < data.size(); ++index)
    {
        auto inputValue = data[index];
        inputNode->SetInput(inputValue);
        std::vector<double> outputVec = model.ComputeOutput(outputNode->output);
        for (int d = 0; d < inputValue.size(); ++d)
        {
            auto expectedOutput = std::exp(inputValue[d]);
            testing::ProcessTest("Testing UnaryOperationNode compute", testing::IsEqual(outputVec[d], expectedOutput));
        }
    }
}

void TestBinaryOperationNodeCompute()
{
    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(data[0].size());
    auto outputNode = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, inputNode->output, emitters::BinaryOperationType::add);

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

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dim);
    model.AddNode<nodes::LinearPredictorNode>(inputNode->output, predictor);
    // TODO: finish this test
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
    testing::ProcessTest("Testing DemultiplexerNode compute", testing::IsEqual(outputVec, { 5.0, 0 }));

    selectorNode->SetInput(true); // output[1] should get the input
    outputVec = model.ComputeOutput(muxNode->output);
    testing::ProcessTest("Testing DemultiplexerNode compute", testing::IsEqual(outputVec, { 0.0, 5.0 }));
}

// Callbacks
struct SourceNodeTester
{
    void Initialize(const std::vector<std::vector<double>>& inputSeries)
    {
        it = inputSeries.begin();
        end = inputSeries.end();
        assert(it != end);
    }

    bool InputCallback(std::vector<double>& input)
    {
        if (it == end)
        {
            return false;
        }
        input = *it;
        it++;
        return true;
    }

private:
    std::vector<std::vector<double>>::const_iterator it;
    std::vector<std::vector<double>>::const_iterator end;
} sourceNodeTester;

// Needed for compile-time template non-type parameter deduction
bool SourceNodeTester_InputCallback(std::vector<double>& input)
{
    return sourceNodeTester.InputCallback(input);
}

void TestSourceNodeCompute()
{
    const std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };
    sourceNodeTester.Initialize(data);

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<model::TimeTickType>>(2);
    auto sourceNode = model.AddNode<nodes::SourceNode<double, &SourceNodeTester_InputCallback>>(
        inputNode->output, data[0].size());

    for (const auto& inputValue : data)
    {
        using namespace std::chrono_literals;

        const model::TimeTickType now = std::chrono::steady_clock::now().time_since_epoch().count();
        std::vector<model::TimeTickType> timeInput{ now - 50, now };
        inputNode->SetInput(timeInput);

        auto output = model.ComputeOutput(sourceNode->output);
        testing::ProcessTest("Testing SourceNode output", testing::IsEqual(output, inputValue));

        std::this_thread::sleep_for(2ms);
    }
}

void TestSinkNodeCompute()
{
    const std::vector<std::vector<double>> data = { { 12 }, { 10 }, { 8 }, { 6 }, { 4 }, { 2 } };
    std::vector<std::vector<double>> results;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto sinkNode = model.AddNode<nodes::SinkNode<double>>(inputNode->output, [&results](const std::vector<double>& values) {
        results.push_back(values);
    });

    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        model.ComputeOutput(sinkNode->output);
    }
    testing::ProcessTest("Testing SinkNode output", testing::IsEqual(data, results));
}

//
// Node refinements
//

void TestMovingAverageNodeRefine()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto meanNode = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    model::TransformContext context;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    auto refinedInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto refinedOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ meanNode->output });
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

void TestSimpleForestPredictorNodeRefine()
{
    // define some abbreviations
    using SplitAction = predictors::SimpleForestPredictor::SplitAction;
    using SplitRule = predictors::SingleElementThresholdPredictor;
    using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;

    // build a forest
    predictors::SimpleForestPredictor forest;
    auto root = forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.3 }, EdgePredictorVector{ -1.0, 1.0 } });
    forest.Split(SplitAction{ forest.GetChildId(root, 0), SplitRule{ 1, 0.6 }, EdgePredictorVector{ -2.0, 2.0 } });
    forest.Split(SplitAction{ forest.GetChildId(root, 1), SplitRule{ 2, 0.9 }, EdgePredictorVector{ -4.0, 4.0 } });
    forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.2 }, EdgePredictorVector{ -3.0, 3.0 } });

    // build the model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto simpleForestPredictorNode = model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);

    // refine
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    auto refinedInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto refinedOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ simpleForestPredictorNode->output });
    auto refinedTreeOutputsElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ simpleForestPredictorNode->treeOutputs });
    auto refinedEdgeIndicatorVectorElements = transformer.GetCorrespondingOutputs(model::PortElements<bool>{ simpleForestPredictorNode->edgeIndicatorVector });

    // check equivalence
    inputNode->SetInput({ 0.18, 0.5, 0.0 });
    refinedInputNode->SetInput({ 0.18, 0.5, 0.0 });
    auto outputValue = model.ComputeOutput(simpleForestPredictorNode->output)[0];
    auto treeOutputsValue = model.ComputeOutput(simpleForestPredictorNode->treeOutputs);
    auto edgeIndicatorVectorValue = model.ComputeOutput(simpleForestPredictorNode->edgeIndicatorVector);

    auto refinedOutputValue = refinedModel.ComputeOutput(refinedOutputElements)[0];
    auto refinedTreeOutputsValue = refinedModel.ComputeOutput(refinedTreeOutputsElements);
    auto refinedEdgeIndicatorVectorValue = refinedModel.ComputeOutput(refinedEdgeIndicatorVectorElements);

    //  expected output is -3.0
    testing::ProcessTest("Testing SimpleForestPredictorNode refine (output)", testing::IsEqual(outputValue, refinedOutputValue));
    testing::ProcessTest("Testing SimpleForestPredictorNode refine (treeOutputs)", testing::IsEqual(treeOutputsValue, refinedTreeOutputsValue));
    testing::ProcessTest("Testing SimpleForestPredictorNode refine (edgeIndicatorVector)", testing::IsEqual(edgeIndicatorVectorValue, refinedEdgeIndicatorVectorValue));
}

void TestLinearPredictorNodeRefine()
{
    // make a linear predictor
    size_t dim = 3;
    predictors::LinearPredictor predictor(dim);
    predictor.GetBias() = 2.0;
    predictor.GetWeights() = math::ColumnVector<double>{ 3.0, 4.0, 5.0 };

    // make a model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto linearPredictorNode = model.AddNode<nodes::LinearPredictorNode>(inputNode->output, predictor);

    // refine the model
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto newModel = transformer.RefineModel(model, context);

    // check for equality
    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto newOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ linearPredictorNode->output });
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
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
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

void TestDTWDistanceNodeCompute()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto prototype = GetNextSlidePrototype();
    auto dtwNode = model.AddNode<nodes::DTWDistanceNode<double>>(inputNode->output, prototype);

    //
    auto prototypeLength = prototype.size();
    size_t numSamples = 200;
    size_t increment = 3;
    for (int index = 0; index < numSamples; ++index)
    {
        auto sampleIndex = (index * increment) % prototypeLength;
        auto inputValue = prototype[sampleIndex];
        inputNode->SetInput(inputValue);
        std::vector<double> outputVec = model.ComputeOutput(dtwNode->output);
        std::cout << "[" << sampleIndex << "]: \t" << outputVec[0] << std::endl;
        if (sampleIndex + increment >= prototypeLength) std::cout << std::endl;
    }
}

void TestMatrixVectorProductRefine()
{
    math::ColumnMatrix<double> w(2, 3);
    w(0, 0) = 1.0;
    w(0, 1) = 0.2;
    w(0, 2) = 0.3;
    w(1, 0) = 0.3;
    w(1, 1) = 0.7;
    w(1, 2) = 0.5;

    std::vector<double> input = { 1, 2, 3 };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(input.size());

    inputNode->SetInput(input);

    auto matrixVectorProductNode = model.AddNode<nodes::MatrixVectorProductNode<double, math::MatrixLayout::columnMajor>>(inputNode->output, w);

    model::TransformContext context;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    auto refinedInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto refinedOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ matrixVectorProductNode->output });

    refinedInputNode->SetInput(input);

    auto refinedOutput = refinedModel.ComputeOutput(refinedOutputElements);
    auto computeOutput = model.ComputeOutput(matrixVectorProductNode->output);

    testing::ProcessTest("Testing matrix vector product node refine", testing::IsEqual(refinedOutput, computeOutput));
}

void TestProtoNNPredictorNode()
{
    using ExampleType = predictors::ProtoNNPredictor::DataVectorType;

    size_t dim = 5, projectedDim = 4, numPrototypes = 3, numLabels = 2;
    double gamma = 0.3;
    predictors::ProtoNNPredictor protonnPredictor(dim, projectedDim, numPrototypes, numLabels, gamma);

    // projectedDim * dim
    auto W = protonnPredictor.GetProjectionMatrix().GetReference();
    W(0, 0) = 0.4;
    W(0, 1) = 0.5;
    W(0, 2) = 0.1;
    W(0, 3) = 0.1;
    W(0, 4) = 0.1;
    W(1, 0) = 0.1;
    W(1, 1) = 0.4;
    W(1, 2) = 0.8;
    W(1, 3) = 0.2;
    W(1, 4) = 0.5;
    W(2, 0) = 0.2;
    W(2, 1) = 0.1;
    W(2, 2) = 0.7;
    W(2, 3) = 0.3;
    W(2, 4) = 0.4;
    W(3, 0) = 0.3;
    W(3, 1) = 0.3;
    W(3, 2) = 0.2;
    W(3, 3) = 0.5;
    W(3, 4) = 0.2;

    // projectedDim * numPrototypes
    auto B = protonnPredictor.GetPrototypes().GetReference();
    B(0, 0) = 0.1;
    B(0, 1) = 0.2;
    B(0, 2) = 0.3;
    B(1, 0) = 0.8;
    B(1, 1) = 0.7;
    B(1, 2) = 0.6;
    B(2, 0) = 0.4;
    B(2, 1) = 0.6;
    B(2, 2) = 0.2;
    B(3, 0) = 0.2;
    B(3, 1) = 0.1;
    B(3, 2) = 0.3;

    // numLabels * numPrototypes
    auto Z = protonnPredictor.GetLabelEmbeddings().GetReference();
    Z(0, 0) = 0.1;
    Z(0, 1) = 0.3, Z(0, 2) = 0.2;
    Z(1, 0) = 0.2;
    Z(1, 1) = 0.4, Z(1, 2) = 0.8;

    std::vector<double> input = { 0.2, 0.5, 0.6, 0.8, 0.1 };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(input.size());

    inputNode->SetInput(input);

    auto protonnPredictorNode = model.AddNode<nodes::ProtoNNPredictorNode>(inputNode->output, protonnPredictor);

    model::TransformContext context;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    auto refinedInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto refinedScoreOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ protonnPredictorNode->outputScore });
    auto refinedLabelOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<int>{ protonnPredictorNode->outputLabel });

    refinedInputNode->SetInput(input);

    auto refinedScoreOutput = refinedModel.ComputeOutput(refinedScoreOutputElements)[0];
    auto refinedLabelOutput = refinedModel.ComputeOutput(refinedLabelOutputElements)[0];

    auto computeScoreOutput = model.ComputeOutput(protonnPredictorNode->outputScore)[0];
    auto computeLabelOutput = model.ComputeOutput(protonnPredictorNode->outputLabel)[0];

    testing::ProcessTest("Testing protonnPredictor node refine", testing::IsEqual(refinedLabelOutput, computeLabelOutput));
    testing::ProcessTest("Testing protonnPredictor node refine", testing::IsEqual(refinedScoreOutput, computeScoreOutput));
}
