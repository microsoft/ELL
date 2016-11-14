//
// Model tests
//

#include "TypedMap_test.h"
#include "Model_test.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"
#include "PortElements.h"
#include "TypedMap.h"


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
#include <cassert>
#include <iostream>
#include <tuple>

//
//
//
namespace emll
{
void TestTypedMapCreate()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);

    auto map = model::MakeMap(model,
                              std::make_tuple(model::MakeNamedInput("doubleInput", inputNodes[0])),
                              std::make_tuple(model::MakeNamedOutput("doubleOutput", outputNodes[0]->output)));
}

void TestTypedMapCompute()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::MakeMap(model,
                              std::make_tuple(model::MakeNamedInput("doubleInput", inputNodes[0])),
                              std::make_tuple(model::MakeNamedOutput("doubleOutput", outputNodes[0]->output)));

    assert(inputNodes.size() == 1);

    auto input = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                   { 4.0, 5.0, 6.0 },
                                                   { 7.0, 8.0, 9.0 },
                                                   { 10.0, 11.0, 12.0 } };
    decltype(map.ComputeOutput()) result;
    for (const auto& inVec : input)
    {
        map.SetInputValues(inVec);
        result = map.ComputeOutput();
    }

    auto resultValues = std::get<0>(result);

    testing::ProcessTest("Testing min value", testing::IsEqual(resultValues[0], 8.5));
    testing::ProcessTest("Testing max value", testing::IsEqual(resultValues[1], 10.5));
}

void TestTypedMapRefine()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);

    auto map1 = model::MakeMap(model,
                               std::make_tuple(model::MakeNamedInput("doubleInput", inputNodes[0])),
                               std::make_tuple(model::MakeNamedOutput("doubleOutput", outputNodes[0]->output)));
    auto map2 = model::MakeMap(model,
                               std::make_tuple(model::MakeNamedInput("doubleInput", inputNodes[0])),
                               std::make_tuple(model::MakeNamedOutput("doubleOutput", outputNodes[0]->output)));

    model::TransformContext context{ common::IsNodeCompilable() };
    map2.Refine(context);

    auto input = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                   { 4.0, 5.0, 6.0 },
                                                   { 7.0, 8.0, 9.0 },
                                                   { 10.0, 11.0, 12.0 } };
    decltype(map1.ComputeOutput()) result1;
    decltype(map2.ComputeOutput()) result2;
    for (const auto& inVec : input)
    {
        map1.SetInputValues(inVec);
        map2.SetInputValues(inVec);

        result1 = map1.ComputeOutput();
        result2 = map2.ComputeOutput();
    }

    // make sure they're the same
    auto resultValues1 = std::get<0>(result1);
    auto resultValues2 = std::get<0>(result2);
    testing::ProcessTest("Testing refined map compute", testing::IsEqual(resultValues1, resultValues2));
}

void TestNamedInputOutput()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();

    assert(inputNodes.size() == 1);
    assert(outputNodes.size() == 1);

    auto map = model::MakeMap(model,
                              std::make_tuple(model::MakeNamedInput("doubleInput", inputNodes[0])),
                              std::make_tuple(model::MakeNamedOutput("doubleOutput", outputNodes[0]->output)));

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

    testing::ProcessTest("Testing named input / output", testing::IsEqual(resultValues[0], 8.5) && testing::IsEqual(resultValues[1], 10.5));
}

void TestTypedMapSerialization()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    auto map = model::MakeMap(model,
                              std::make_tuple(model::MakeNamedInput("doubleInput", inputNodes[0])),
                              std::make_tuple(model::MakeNamedOutput("doubleOutput", outputNodes[0]->output)));

    std::stringstream outStream;
    utilities::XmlArchiver archiver(outStream);
    archiver << map;

    // Now read it back in
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    std::stringstream inStream(outStream.str());
    utilities::XmlUnarchiver unarchiver(inStream, context);
    model::TypedMap<std::tuple<double>, std::tuple<double>> map2;
    unarchiver >> map2;

    // Now read it back in --- as a DynamicMap
    context = utilities::SerializationContext{};
    common::RegisterNodeTypes(context);
    context.GetTypeFactory().AddType<model::DynamicMap, model::TypedMap<std::tuple<double>, std::tuple<double>>>();
    inStream.seekg(0);
    utilities::XmlUnarchiver unarchiver2(inStream, context);
    std::unique_ptr<model::DynamicMap> dmap;
    unarchiver2 >> dmap;
}

void TestComplexMap()
{
    auto model = GetComplexModel();
    auto doubleInputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto boolInputNodes = model.GetNodesByType<model::InputNode<bool>>();
    auto doubleOutputNodes = model.GetNodesByType<model::OutputNode<double>>();
    auto boolOutputNodes = model.GetNodesByType<model::OutputNode<bool>>();

    assert(doubleInputNodes.size() == 1);
    assert(boolInputNodes.size() == 1);
    assert(doubleOutputNodes.size() == 1);
    assert(boolOutputNodes.size() == 1);

    auto map = model::MakeMap(model,
                              std::make_tuple(model::MakeNamedInput("doubleInput", doubleInputNodes[0]),
                                              model::MakeNamedInput("boolInput", boolInputNodes[0])),
                              std::make_tuple(model::MakeNamedOutput("doubleOutput", doubleOutputNodes[0]->output),
                                              model::MakeNamedOutput("boolOutput", boolOutputNodes[0]->output)));

    std::stringstream outStream;
    utilities::XmlArchiver archiver(outStream);
    archiver << map;

    // Now read it back in
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    std::stringstream inStream(outStream.str());
    utilities::XmlUnarchiver unarchiver(inStream, context);
    model::TypedMap<std::tuple<double>, std::tuple<double>> map2;
    unarchiver >> map2;

    // Now read it back in --- as a DynamicMap
    context = utilities::SerializationContext{};
    common::RegisterNodeTypes(context);
    context.GetTypeFactory().AddType<model::DynamicMap, model::TypedMap<std::tuple<double, bool>, std::tuple<double, bool>>>();
    inStream.seekg(0);
    utilities::XmlUnarchiver unarchiver2(inStream, context);
    std::unique_ptr<model::DynamicMap> dmap;
    unarchiver2 >> dmap;
}
}
