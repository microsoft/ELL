////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizerTest.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// #include "ModelTestUtilities.h"

// model
#include "InputNode.h"
#include "IRMapCompiler.h"
#include "Map.h"
#include "ModelOptimizer.h"
#include "PortMemoryLayout.h"

// nodes
#include "BroadcastFunctionNode.h"
#include "ConstantNode.h"

// passes
#include "FuseLinearOperationsPass.h"
#include "StandardPasses.h"

// testing
#include "testing.h"

// stl
#include <iostream>

using namespace ell;

//
// Utility functions
//
void PrintModel(const model::Model& model)
{
    model.Print(std::cout);
}

void PrintMap(const model::Map& map)
{
    map.GetModel().Print(std::cout);
}

//
// Tests
//
void TestModelOptimizer()
{
    using ValueType = float;

    // Create a model
    model::Model model;
    int numRows = 8;
    int numColumns = 8;
    int numChannels = 4;
    std::vector<ValueType> scale(numChannels);
    std::vector<ValueType> bias(numChannels);
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(numRows * numColumns * numChannels);
    auto scaleNode = model.AddNode<nodes::ConstantNode<ValueType>>(scale);
    auto biasNode = model.AddNode<nodes::ConstantNode<ValueType>>(bias);
    model::PortMemoryLayout inputLayout({ 8, 8, 4 });
    model::PortMemoryLayout outputLayout({ 8, 8, 4 });
    auto functionNode1 = model.AddNode<nodes::BroadcastLinearFunctionNode<ValueType>>(inputNode->output, inputLayout, scaleNode->output, biasNode->output, 2, outputLayout);
    auto functionNode2 = model.AddNode<nodes::BroadcastLinearFunctionNode<ValueType>>(functionNode1->output, inputLayout, scaleNode->output, biasNode->output, 2, outputLayout);
    auto functionNode3 = model.AddNode<nodes::BroadcastLinearFunctionNode<ValueType>>(functionNode2->output, inputLayout, scaleNode->output, biasNode->output, 2, outputLayout);

    // Make a map from it
    model::Map map(model, { { "input", inputNode } }, { { "output", functionNode3->output } });
    auto oldSize = map.GetModel().Size();

    // Optimize it
    model::ModelOptimizer optimizer;
    optimizer.AddPass(std::make_unique<passes::FuseLinearOperationsPass>());
    map.Optimize(optimizer);
    auto newSize = map.GetModel().Size();

    testing::ProcessTest("Testing model optimizer", oldSize == 6 || newSize == 4);
}

void TestModelCompilePlusOptimize()
{
    using ValueType = float;

    // Create a model
    model::Model model;
    int numRows = 8;
    int numColumns = 8;
    int numChannels = 4;
    std::vector<ValueType> scale(numChannels);
    std::vector<ValueType> bias(numChannels);
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(numRows * numColumns * numChannels);
    auto scaleNode = model.AddNode<nodes::ConstantNode<ValueType>>(scale);
    auto biasNode = model.AddNode<nodes::ConstantNode<ValueType>>(bias);
    model::PortMemoryLayout inputLayout({ 8, 8, 4 });
    model::PortMemoryLayout outputLayout({ 8, 8, 4 });
    auto functionNode1 = model.AddNode<nodes::BroadcastLinearFunctionNode<ValueType>>(inputNode->output, inputLayout, scaleNode->output, biasNode->output, 2, outputLayout);
    auto functionNode2 = model.AddNode<nodes::BroadcastLinearFunctionNode<ValueType>>(functionNode1->output, inputLayout, scaleNode->output, biasNode->output, 2, outputLayout);
    auto functionNode3 = model.AddNode<nodes::BroadcastLinearFunctionNode<ValueType>>(functionNode2->output, inputLayout, scaleNode->output, biasNode->output, 2, outputLayout);

    // Make a map from it
    model::Map map(model, { { "input", inputNode } }, { { "output", functionNode3->output } });
    auto oldSize = map.GetModel().Size();

    // Initialize pass registry
    passes::AddStandardPassesToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    auto newSize = compiledMap.GetModel().Size();

    testing::ProcessTest("Testing compiled model optimizer", oldSize == 6 || newSize == 4);
}
