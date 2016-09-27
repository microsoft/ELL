//
// Model tests
//

#include "DynamicMap_test.h"
#include "Model_test.h"

// model
#include "Model.h"
#include "DynamicMap.h"
#include "PortElements.h"
#include "InputNode.h"
#include "OutputNode.h"

// nodes
#include "ExtremalValueNode.h"
#include "MovingAverageNode.h"

// common
#include "IsNodeCompilable.h"
#include "LoadModel.h" // for RegisterNodeTypes

// utilities
#include "XmlArchiver.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <cassert>
#include <tuple>
#include <sstream>

//
// Test code for dynamic map class
//
namespace emll
{

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

    testing::ProcessTest("Testing map compute", testing::IsEqual(resultValues[0], 8.5) && testing::IsEqual(resultValues[1], 10.5));
}

void TestDynamicMapRefine()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);

    auto map1 = model::DynamicMap(model, { { "doubleInput", inputNodes[0] } }, { { "doubleOutput", outputNodes[0]->output } });
    auto map2 = model::DynamicMap(model, { { "doubleInput", inputNodes[0] } }, { { "doubleOutput", outputNodes[0]->output } });

    model::TransformContext context{ common::IsNodeCompilable() };
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
    utilities::XmlArchiver archiver(outStream);
    archiver << map;

    // Now read it back in
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    std::stringstream inStream(outStream.str());
    utilities::XmlUnarchiver unarchiver(inStream, context);
    model::DynamicMap map2;
    unarchiver >> map2;
}
}
