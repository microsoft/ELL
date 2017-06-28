////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DynamicMap_test.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DynamicMap_test.h"
#include "ModelTestUtilities.h"

// data
#include "DenseDataVector.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"
#include "PortElements.h"
#include "SteppableMap.h"

// nodes
#include "ExtremalValueNode.h"
#include "MovingAverageNode.h"
#include "SourceNode.h"

// common
#include "LoadModel.h" // for RegisterNodeTypes

// utilities
#include "JsonArchiver.h"

// testing
#include "testing.h"

// stl
#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <tuple>

using namespace ell;

//
// Test code for dynamic map class
//

// Returns a model with 1 3-dimensional double input and 1 2-dimensional double output
void TestDynamicMapCreate()
{
    auto model = GetSimpleModel();

    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::DynamicMap(model, { { "doubleInput", inputNodes[0] } }, { { "doubleOutput", outputNodes[0]->output } });
}

void TestDynamicMapCompute()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::DynamicMap(model, { { "doubleInput", inputNodes[0] } }, { { "doubleOutput", outputNodes[0]->output } });

    assert(inputNodes.size() == 1);

    auto input = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                   { 4.0, 5.0, 6.0 },
                                                   { 7.0, 8.0, 9.0 },
                                                   { 10.0, 11.0, 12.0 } };
    std::vector<double> resultValues;
    for (const auto& inVec : input)
    {
        map.SetInputValue("doubleInput", inVec);
        resultValues = map.ComputeOutput<double>("doubleOutput");
    }

    testing::ProcessTest("Testing map compute 1", testing::IsEqual(resultValues[0], 8.5) && testing::IsEqual(resultValues[1], 10.5));
}

void TestDynamicMapComputeDataVector()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::DynamicMap(model, { { "doubleInput", inputNodes[0] } }, { { "doubleOutput", outputNodes[0]->output } });

    assert(inputNodes.size() == 1);

    auto signal = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                    { 4.0, 5.0, 6.0 },
                                                    { 7.0, 8.0, 9.0 },
                                                    { 10.0, 11.0, 12.0 } };
    std::vector<double> resultValues;
    for (const auto& sample : signal)
    {
        data::DoubleDataVector sampleVec(sample);
        map.SetInputValue("doubleInput", sampleVec);
        resultValues = map.ComputeOutput<double>("doubleOutput");
    }

    testing::ProcessTest("Testing map compute 2", testing::IsEqual(resultValues[0], 8.5) && testing::IsEqual(resultValues[1], 10.5));
}

void TestDynamicMapRefine()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);

    auto map1 = model::DynamicMap(model, { { "doubleInput", inputNodes[0] } }, { { "doubleOutput", outputNodes[0]->output } });
    auto map2 = model::DynamicMap(model, { { "doubleInput", inputNodes[0] } }, { { "doubleOutput", outputNodes[0]->output } });

    model::TransformContext context;
    map2.Refine(context);

    auto input = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                   { 4.0, 5.0, 6.0 },
                                                   { 7.0, 8.0, 9.0 },
                                                   { 10.0, 11.0, 12.0 } };
    std::vector<double> resultValues1;
    std::vector<double> resultValues2;
    for (const auto& inVec : input)
    {
        map1.SetInputValue("doubleInput", inVec);
        map2.SetInputValue("doubleInput", inVec);

        resultValues1 = map1.ComputeOutput<double>("doubleOutput");
        resultValues2 = map2.ComputeOutput<double>("doubleOutput");
    }

    // make sure they're the same
    testing::ProcessTest("Testing refined map compute", testing::IsEqual(resultValues1, resultValues2));
}

void TestDynamicMapSerialization()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    auto map = model::DynamicMap(model, { { "doubleInput", inputNodes[0] } }, { { "doubleOutput", outputNodes[0]->output } });

    std::stringstream outStream;
    utilities::JsonArchiver archiver(outStream);
    archiver << map;

    std::cout << "Archived map" << std::endl;
    std::cout << outStream.str() << std::endl;

    // Now read it back in
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    common::RegisterMapTypes(context);
    std::stringstream inStream(outStream.str());
    utilities::JsonUnarchiver unarchiver(inStream, context);
    model::DynamicMap map2;
    unarchiver >> map2;
}

InputCallbackTester<double> g_stepDataCallbackTester;
bool TestSteppableMapCompute_DataCallback(std::vector<double>& input)
{
    return g_stepDataCallbackTester.InputCallback(input);
}
void TestSteppableMapCompute()
{
    model::Model model;
    auto in = model.AddNode<model::InputNode<model::TimeTickType>>(2); // time signal
    auto source = model.AddNode<nodes::SourceNode<double, &TestSteppableMapCompute_DataCallback>>(in->output, 3); // data signal
    auto minAndArgMin = model.AddNode<nodes::ArgMinNode<double>>(source->output);
    auto maxAndArgMax = model.AddNode<nodes::ArgMaxNode<double>>(source->output);
    auto meanMin = model.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 2);
    auto meanMax = model.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 2);
    auto output = model.AddNode<model::OutputNode<double>>(model::PortElements<double>({ meanMin->output, meanMax->output }));

    std::chrono::milliseconds interval(100);
    auto map = model::SteppableMap<std::chrono::steady_clock>(
        model,
        { { "timeSignal", in } },
        { { "doubleOutput", output->output } },
        interval);

    auto dataSignal = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                        { 4.0, 5.0, 6.0 },
                                                        { 7.0, 8.0, 9.0 },
                                                        { 10.0, 11.0, 12.0 } };
    g_stepDataCallbackTester.Initialize(dataSignal);

    // The time signal is auto-generated by the map based on clock and interval, so not fed into the map here

    // Compute called the first time
    auto resultValues = map.ComputeOutput<double>("doubleOutput");
    testing::ProcessTest("Testing steppable dynamic map compute (first call)", testing::IsEqual(resultValues[0], 0.5) && testing::IsEqual(resultValues[1], 1.5));

    // Compute called early
    auto sleepDuration = map.GetWaitTimeForNextCompute();
    std::cout << "GetWaitTimeForNextCompute duration: " << sleepDuration.count() << " msecs" << std::endl;
    std::this_thread::sleep_for(sleepDuration / 2); // simulate delay < wait time
    resultValues = map.ComputeOutput<double>("doubleOutput");
    testing::ProcessTest("Testing steppable dynamic map compute (early)", testing::IsEqual(resultValues.size(), size_t(0)));

    // Compute called late
    sleepDuration = map.GetWaitTimeForNextCompute();
    std::cout << "Sleep duration: " << sleepDuration.count() << " msecs" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(interval * 2)); // simulate extra delay

    resultValues = map.ComputeOutput<double>("doubleOutput");
    testing::ProcessTest("Testing steppable dynamic map compute (late)", testing::IsEqual(resultValues.size(), size_t(2)));
    std::cout << "Results count: " << resultValues.size() << std::endl;
}
