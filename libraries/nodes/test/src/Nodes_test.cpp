////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Nodes_test.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nodes_test.h"
#include "MeanNode.h"
#include "VarianceNode.h"
#include "DelayNode.h"
#include "AccumulatorNode.h"
#include "BinaryOperationNode.h"
#include "UnaryOperationNode.h"
#include "MagnitudeNode.h"
#include "LinearPredictorNode.h"

// model
#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"

// predictors
#include "LinearPredictor.h"

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

void TestMagnitudeNodeCompute()
{
    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(data[0].size());
    auto outputNode = model.AddNode<nodes::MagnitudeNode<double>>(inputNode->output);

    bool okSize = true;
    bool okResult = true;
    for (int index = 0; index < data.size(); ++index)
    {
        auto inputValue = data[index];
        double expectedOutput = VectorMagnitude(inputValue);

        inputNode->SetInput(inputValue);
        std::vector<double> outputVec = model.GetNodeOutput(outputNode->output);

        testing::ProcessTest("Testing MagnitudeNode output size", testing::IsEqual(outputVec.size(), 1));
        testing::ProcessTest("Testing MagnitudeNode compute", testing::IsEqual(outputVec[0], expectedOutput));
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
        std::vector<double> outputVec = model.GetNodeOutput(outputNode->output);

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
        outputVec = model.GetNodeOutput(outputNode->output);
        if (index >= delay)
        {
            testing::ProcessTest("Testing DelayNode compute", testing::IsEqual(outputVec, data[index - delay]));
        }
    }
}

void TestMeanNodeCompute()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto outputNode = model.AddNode<nodes::MeanNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };
    double expectedOutput = VectorMean({ 7.0, 8.0, 9.0, 10.0 });

    std::vector<double> outputVec;

    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        outputVec = model.GetNodeOutput(outputNode->output);
    }
    testing::ProcessTest("Testing MeanNode compute", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestVarianceNodeCompute()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto outputNode = model.AddNode<nodes::VarianceNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };
    double mean = VectorMean({ 7.0, 8.0, 9.0, 10.0 });
    double expectedOutput = VectorVariance({ 7.0, 8.0, 9.0, 10.0 }, mean);

    std::vector<double> outputVec;

    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        outputVec = model.GetNodeOutput(outputNode->output);
    }
    testing::ProcessTest("Testing VarianceNode compute", testing::IsEqual(outputVec[0], expectedOutput));
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
        std::vector<double> outputVec = model.GetNodeOutput(outputNode->output);

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
        std::vector<double> outputVec = model.GetNodeOutput(outputNode->output);

        for (int d = 0; d < inputValue.size(); ++d)
        {
            auto expectedOutput = 2*inputValue[d];
            testing::ProcessTest("Testing BinaryOperationNode compute", testing::IsEqual(outputVec[d], expectedOutput));
        }
    }
}

//
// Node refinements
//

// TODO: make a generic TestGraphsEqual function that takes 2 graphs, 2 output nodes, and a test message string

void TestMeanNodeRefine()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto meanNode = model.AddNode<nodes::MeanNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } };
    double expectedOutput = VectorMean({ 7.0, 8.0, 9.0, 10.0 });

    model::TransformContext context;
    model::ModelTransformer transformer(context);
    auto newModel = transformer.RefineModel(model);
    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto newOutputPort = transformer.GetCorrespondingOutputPort(meanNode->output);

    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        auto outputVec1 = model.GetNodeOutput(meanNode->output);
        newInputNode->SetInput(inputValue);
        auto outputVec2 = newModel.GetNodeOutput(*newOutputPort);

        testing::ProcessTest("Testing MeanNode refine", testing::IsEqual(outputVec1, outputVec2));
    }
}

void TestLinearPredictorNodeCompute()
{
    const int dim = 10;
    predictors::LinearPredictor predictor(dim);
    // TODO: set it's vector and bias

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dim);
    auto predNode = model.AddNode<nodes::LinearPredictorNode>(inputNode->output, predictor);

    // TODO: test it
}

