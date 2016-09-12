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

// Returns a model with 1 3-dimensional double input and 1 2-dimensional double output
model::Model GetSimpleModel()
{
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto minAndArgMin = g.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto maxAndArgMax = g.AddNode<nodes::ArgMaxNode<double>>(in->output);
    auto meanMin = g.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 2);
    auto meanMax = g.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 2);
    g.AddNode<model::OutputNode<double>>(model::PortElements<double>({ meanMin->output, meanMax->output }));
    return g;
}

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
    std::vector<double> resultValues;
    for (const auto& inVec : input)
    {
        map.SetInput("doubleInput", inVec);
        resultValues = map.ComputeOutput<double>("doubleOutput");
    }

    testing::ProcessTest("Testing named input / output", testing::IsEqual(resultValues[0], 8.5) && testing::IsEqual(resultValues[1], 10.5));

    for (auto x : resultValues)
        std::cout << x << "  ";
    std::cout << std::endl;
}
}
