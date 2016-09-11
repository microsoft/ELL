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
model::Model GetSimpleModel()
{
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto minAndArgMin = g.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto maxAndArgMax = g.AddNode<nodes::ArgMaxNode<double>>(in->output);
    auto meanMin = g.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 8);
    auto meanMax = g.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 8);
    auto meanOutputs = g.AddNode<model::OutputNode<double>>(model::PortElements<double>({ meanMin->output, meanMax->output }));
    return g;
}

void TestMapCreate()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::Map<double>(model, std::make_tuple(MakePortElements(outputNodes[0]->output)), { "doubleOutput" });
}

void TestMapCompute()
{
    auto model = GetSimpleModel();
    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map = model::Map<double>(model, std::make_tuple(MakePortElements(outputNodes[0]->output)), { "doubleOutput" });

    assert(inputNodes.size() == 1);
    auto inputNode = inputNodes[0];
    inputNode->SetInput({1.0, 2.0, 3.0});

    auto resultTuple = map.Compute();
    auto result = std::get<0>(resultTuple);
    std::cout << "Output size: " << result.size() << std::endl;
}

void TestMapRefine()
{
    auto model = GetSimpleModel();
    auto outputNodes = model.GetNodesByType<model::OutputNode<double>>();
    assert(outputNodes.size() == 1);
    auto map1 = model::Map<double>(model, std::make_tuple(MakePortElements(outputNodes[0]->output)), { "doubleOutput" });
    auto map2 = model::Map<double>(model, std::make_tuple(MakePortElements(outputNodes[0]->output)), { "doubleOutput" });

    model::TransformContext context{ common::IsNodeCompilable() };
    map2.Refine(context);


}

}
