////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Nodes_test.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nodes_test.h"
#include "MeanNode.h"

// model
#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"

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
        for(const auto& x: vec) sumSq += (x*x);
        
        return std::sqrt(sumSq);
    }

    double VectorMean(const std::vector<double>& vec)
    {
        double sum = 0.0;
        for(const auto& x: vec) sum += x;
        
        return sum / vec.size();
    }

    double VectorVariance(const std::vector<double>& vec, double mean)
    {
        if(vec.size() == 0) return 0.0;
        double var = 0.0;
        for (auto x : vec)
        {
            double diff = x - mean;
            var += diff*diff;
        }
        return var / vec.size();
    }

    std::ostream& operator<<(std::ostream& out, const std::vector<double> array)
    {
        out << "[";
        for(auto x: array) out << x << "  ";
        out << "]";
        return out;
    }
}

//
// Test functions
//
void TestMagnitudeFeature()
{
    // FeatureSet features;
    // auto inputFeature = features.CreateFeature<InputFeature>(3);
    // auto magnitudeFeature = features.CreateFeature<MagnitudeFeature>(inputFeature);
    // features.SetOutputFeature(magnitudeFeature);
    // features.Reset();

    // std::vector<std::vector<double>> data = {{ 1,2,3 }};
    // double expectedOutput = VectorMagnitude(data.back());

    // // 1) Test Compute()
    // std::vector<double> outputVec;
    // bool hasOutput = false;
    // for(const auto& x: data)
    // {
    //     hasOutput = features.ProcessInputData(x);
    // }

    // if (hasOutput)
    // {
    //     outputVec = static_cast<std::vector<double>>(features.GetOutput());
    // }
    // else
    // {
    //     testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    // }

    // testing::ProcessTest("Testing MagnitudeFeature compute", testing::IsEqual(outputVec[0], expectedOutput));

    // // 2) Create Model / Map and test it
    // layers::Model model;
    // layers::CoordinateList inputCoordinates(0, 3); // What should the input coordinates be???
    // auto outputCoordinates = features.AddToModel(model, inputCoordinates);

    // // std::cout << "Magnitude feature model" << std::endl;
    // // model.Save(std::cout);
    // // std::cout << std::endl;

    // // create a map
    // layers::Map map(model, outputCoordinates);

    // // now run a test vector through it
    // for(const auto& x: data)
    // {
    //     outputVec = map.Compute(x);
    // }
    
    // if (outputVec.size() == 0)
    // {
    //     testing::ProcessTest("Error: no output from MagnitudeFeature", false); 
    // }
    // testing::ProcessTest("Testing MagnitudeFeature model", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestMeanNodeCompute()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto meanNode = model.AddNode<nodes::MeanNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };
    double expectedOutput = VectorMean({7.0, 8.0, 9.0, 10.0}); 

    // 1) Test Compute()
    std::vector<double> outputVec;

    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        outputVec = model.GetNodeOutput(meanNode->output);
    }
    std::cout << "Node compute output: " << outputVec[0] <<", expected: " << expectedOutput << std::endl;
    testing::ProcessTest("Testing MeanNode compute", testing::IsEqual(outputVec[0], expectedOutput));
}

void TestMeanNodeRefine()
{
    const int windowSize = 4;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto meanNode = model.AddNode<nodes::MeanNode<double>>(inputNode->output, windowSize);

    std::vector<std::vector<double>> data = { {1},{2},{3},{4},{5},{6},{7},{8},{9},{10} };
    double expectedOutput = VectorMean({7.0, 8.0, 9.0, 10.0}); 

    model::TransformContext context;
    model::ModelTransformer transformer(context);
    auto newModel = transformer.RefineModel(model);
    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto newOutputPort = transformer.GetCorrespondingOutputPort(meanNode->output);

    // 1) Test Compute()
    for (const auto& inputValue : data)
    {
        inputNode->SetInput(inputValue);
        auto outputVec1 = model.GetNodeOutput(meanNode->output);
        newInputNode->SetInput(inputValue);
        auto outputVec2 = newModel.GetNodeOutput(*newOutputPort);

        testing::ProcessTest("Testing MeanNode compute", testing::IsEqual(outputVec1, outputVec2));
    }
}
