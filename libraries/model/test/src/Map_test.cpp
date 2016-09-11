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
                              { {"doubleInput"} },
                              std::make_tuple(MakePortElements(outputNodes[0]->output)),
                              { {"doubleOutput"} });
}

void TestMapCompute()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::MakeMap(model,
                              std::make_tuple(inputNodes[0]),
                              { {"doubleInput"} },
                              std::make_tuple(MakePortElements(outputNodes[0]->output)),
                              { {"doubleOutput"} });

    assert(inputNodes.size() == 1);

    auto input = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
                                                   { 4.0, 5.0, 6.0 },
                                                   { 7.0, 8.0, 9.0 },
                                                   { 10.0, 11.0, 12.0 } };
    decltype(map.Compute()) result;
    for (const auto& inVec : input)
    {
        map.SetInputs(std::make_tuple(inVec));
        result = map.Compute(); 
    }

    std::cout << "map compute result type: " << utilities::TypeName<decltype(result)>::GetName() << std::endl;
    auto resultValues = std::get<0>(result);
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
                               { {"doubleInput"} },
                               std::make_tuple(MakePortElements(outputNodes[0]->output)),
                               { {"doubleOutput"} });

    auto map2 = model::MakeMap(model,
                               std::make_tuple(inputNodes[0]),
                               { {"doubleInput"} },
                               std::make_tuple(MakePortElements(outputNodes[0]->output)),
                               { {"doubleOutput"} });

    model::TransformContext context{ common::IsNodeCompilable() };
    map2.Refine(context);

    auto inputVec = std::vector<double>{ 1.0, 2.0, 3.0 };
    map1.SetInputs(std::make_tuple(inputVec));
    map2.SetInputs(std::make_tuple(inputVec));
    
    auto input = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },
        { 4.0, 5.0, 6.0 },
        { 7.0, 8.0, 9.0 },
        { 10.0, 11.0, 12.0 } };
    decltype(map1.Compute()) result1;
    decltype(map2.Compute()) result2;
    for (const auto& inVec : input)
    {
        map1.SetInputs(std::make_tuple(inVec));
        map2.SetInputs(std::make_tuple(inVec));
        result1 = map1.Compute();
        result2 = map2.Compute();
    }

    // make sure they're the same
    auto resultValues1 = std::get<0>(result1);
    for (auto x : resultValues1)
        std::cout << x << "  ";
    std::cout << std::endl;

    auto resultValues2 = std::get<0>(result2);
    for (auto x : resultValues2)
        std::cout << x << "  ";
    std::cout << std::endl;

}
}
