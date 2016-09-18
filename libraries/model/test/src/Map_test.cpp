//
// Model tests
//

#include "Map_test.h"
#include "Model_test.h"

// model
#include "Model.h"
#include "Map.h"
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

//
//
//
namespace emll
{
void TestMapCreate()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::MakeMap(model,
                              std::make_tuple(inputNodes[0]),
                              { { "doubleInput" } },
                              std::make_tuple(MakePortElements(outputNodes[0]->output)),
                              { { "doubleOutput" } });
}

void TestMapCompute()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::MakeMap(model,
                              std::make_tuple(inputNodes[0]),
                              { { "doubleInput" } },
                              std::make_tuple(MakePortElements(outputNodes[0]->output)),
                              { { "doubleOutput" } });

    assert(inputNodes.size() == 1);

    auto input = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                   { 4.0, 5.0, 6.0 },
                                                   { 7.0, 8.0, 9.0 },
                                                   { 10.0, 11.0, 12.0 } };
    decltype(map.ComputeOutput()) result;
    for (const auto& inVec : input)
    {
        map.SetInputs(inVec);
        result = map.ComputeOutput();
    }

    auto resultValues = std::get<0>(result);

    testing::ProcessTest("Testing min value", testing::IsEqual(resultValues[0], 8.5));
    testing::ProcessTest("Testing max value", testing::IsEqual(resultValues[1], 10.5));

    for (auto x : resultValues)
        std::cout << x << "  ";
    std::cout << std::endl;
}

void TestMapRefine()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);

    auto map1 = model::MakeMap(model,
                               std::make_tuple(inputNodes[0]),
                               { { "doubleInput" } },
                               std::make_tuple(MakePortElements(outputNodes[0]->output)),
                               { { "doubleOutput" } });

    auto map2 = model::MakeMap(model,
                               std::make_tuple(inputNodes[0]),
                               { { "doubleInput" } },
                               std::make_tuple(MakePortElements(outputNodes[0]->output)),
                               { { "doubleOutput" } });

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
        map1.SetInputs(inVec);
        map2.SetInputs(inVec);

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

    std::cout << "num input/output nodes: " << inputNodes.size() << ", " << outputNodes.size() << std::endl;

    assert(inputNodes.size() == 1);
    assert(outputNodes.size() == 1);

    auto map = model::MakeMap(model,
                              std::make_tuple(inputNodes[0]),
                              { { "doubleInput" } },
                              std::make_tuple(MakePortElements(outputNodes[0]->output)),
                              { { "doubleOutput" } });

    auto input = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                   { 4.0, 5.0, 6.0 },
                                                   { 7.0, 8.0, 9.0 },
                                                   { 10.0, 11.0, 12.0 } };
    std::vector<double> resultValues;
    for (const auto& inVec : input)
    {
        map.SetInput("doubleInput", inVec);
        resultValues = map.ComputeOutput<double>("doubleOutput");
    }

    testing::ProcessTest("Testing named input / output", testing::IsEqual(resultValues[0], 8.5) && testing::IsEqual(resultValues[1], 10.5));
}

void TestMapSerialization()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    auto map = model::MakeMap(model,
                              std::make_tuple(inputNodes[0]),
                              { { "doubleInput" } },
                              std::make_tuple(MakePortElements(outputNodes[0]->output)),
                              { { "doubleOutput" } });

    std::stringstream outStream;
    utilities::XmlArchiver archiver(outStream);
    archiver << map;

    // std::cout << "\nArchived version of map:" << std::endl;
    // std::cout << outStream.str();

    // Now read it back in
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    std::stringstream inStream(outStream.str());
    utilities::XmlUnarchiver unarchiver(inStream, context);
    model::Map<std::tuple<double>, std::tuple<double>> map2;
    unarchiver >> map2;

    // Now read it back in --- as a DynamicMap
    context = utilities::SerializationContext{};
    common::RegisterNodeTypes(context);
    context.GetTypeFactory().AddType<model::DynamicMap, model::Map<std::tuple<double>, std::tuple<double>>>();
    inStream.seekg(0);
    utilities::XmlUnarchiver unarchiver2(inStream, context);
    std::unique_ptr<model::DynamicMap> dmap;
    unarchiver2 >> dmap;
}

void TestComplexMap()
{
    std::cout << "\nTestComplexMap" << std::endl;
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
                              std::make_tuple(doubleInputNodes[0], boolInputNodes[0]),
                              { { "doubleInput", "boolInput" } },
                              std::make_tuple(MakePortElements(doubleOutputNodes[0]->output), MakePortElements(boolOutputNodes[0]->output)),
                              { { "doubleOutput", "boolOutput" } });

    std::stringstream outStream;
    utilities::XmlArchiver archiver(outStream);
    archiver << map;

    // std::cout << "\nArchived version of map:" << std::endl;
    // std::cout << outStream.str();

    // Now read it back in
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    std::stringstream inStream(outStream.str());
    utilities::XmlUnarchiver unarchiver(inStream, context);
    model::Map<std::tuple<double>, std::tuple<double>> map2;
    unarchiver >> map2;

    // Now read it back in --- as a DynamicMap
    context = utilities::SerializationContext{};
    common::RegisterNodeTypes(context);
    context.GetTypeFactory().AddType<model::DynamicMap, model::Map<std::tuple<double, bool>, std::tuple<double, bool>>>();
    inStream.seekg(0);
    utilities::XmlUnarchiver unarchiver2(inStream, context);
    std::unique_ptr<model::DynamicMap> dmap;
    unarchiver2 >> dmap;
}
}
