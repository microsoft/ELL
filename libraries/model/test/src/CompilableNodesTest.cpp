////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodesTest.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableNodesTest.h"
#include "../clang/DotProductIR.h"
#include "ModelTestUtilities.h"

// model
#include "CompiledMap.h"
#include "DynamicMap.h"
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRCompiledMap.h"
#include "IREmitter.h"
#include "IRMapCompiler.h"
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// nodes
#include "AccumulatorNode.h"
#include "BatchNormalizationLayerNode.h"
#include "BiasLayerNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "ConstantNode.h"
#include "DTWDistanceNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "IRNode.h"
#include "MultiplexerNode.h"
#include "NeuralNetworkPredictorNode.h"
#include "SourceNode.h"
#include "SumNode.h"
#include "TypeCastNode.h"
#include "UnaryOperationNode.h"

// predictors
#include "BatchNormalizationLayer.h"
#include "BiasLayer.h"
#include "InputLayer.h"
#include "NeuralNetworkPredictor.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <ostream>
#include <string>

using namespace ell;

namespace
{
size_t GetShapeSize(const math::Triplet& shape)
{
    return shape[0] * shape[1] * shape[2];
}
}

void TestCompileIsEqual()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(2);

    // Error: passing in a single-element PortElements for the inputs to the BinaryPredicateNode causes us to think it's a scalar and pass in the first value of the port, not the selected one
    auto predicateNode = model.AddNode<nodes::BinaryPredicateNode<double>>(model::PortElements<double>{ inputNode->output, 0 }, model::PortElements<double>{ inputNode->output, 1 }, emitters::BinaryPredicateType::equal);
    auto outputNode = model.AddNode<model::OutputNode<bool>>(predicateNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 1 }, { 2, 2 }, { 1, 3 }, { 1, 4 }, { 5, 5 }, { 1, 4 }, { 3, 3 }, { 2, 2 }, { 1, 0 } };
    PrintIR(compiledMap);
    VerifyCompiledOutput(map, compiledMap, signal, "IsEqual model");
}

void TestCompilableScalarOutputNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto outputNode = model.AddNode<model::OutputNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 4 }, { 3 }, { 2 }, { 1 } };
    PrintIR(compiledMap);
    VerifyCompiledOutput(map, compiledMap, signal, "scalar OutputNode");
}

void TestCompilableVectorOutputNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto outputNode = model.AddNode<model::OutputNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    PrintIR(compiledMap);
    VerifyCompiledOutput(map, compiledMap, signal, "VectorOutputNode");
}

void TestCompilableAccumulatorNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto accumNode = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", accumNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMapA = compiler.Compile(map);
    auto compiledMap = std::move(compiledMapA);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "AccumulatorNode");
}

void TestCompilableConstantNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto dotNode = model.AddNode<nodes::DotProductNode<double>>(inputNode->output, constantNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", dotNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "ConstantNode");
}

void TestCompilableDotProductNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto dotNode = model.AddNode<nodes::DotProductNode<double>>(inputNode->output, constantNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", dotNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "DotProductNode");
}

void TestCompilableDelayNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto delayNode = model.AddNode<nodes::DelayNode<double>>(inputNode->output, 8);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", delayNode->output } });
    model::IRMapCompiler compiler;

    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "DelayNode");
}

void TestCompilableDTWDistanceNode()
{
    model::Model model;
    std::vector<std::vector<double>> prototype = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto dtwNode = model.AddNode<nodes::DTWDistanceNode<double>>(inputNode->output, prototype);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", dtwNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "DTWDistanceNode");
}

class LabeledPrototype
{
public:
    LabeledPrototype() = default;
    LabeledPrototype(const LabeledPrototype&) = default;
    LabeledPrototype(int label, const std::vector<std::vector<double>>& prototype)
        : _label(label), _prototype(prototype){};
    int Label() const { return _label; }
    size_t Dimension() const { return _prototype[0].size(); }
    std::vector<std::vector<double>> Prototype() const { return _prototype; }

private:
    int _label = 0;
    std::vector<std::vector<double>> _prototype;
};

model::DynamicMap GenerateMulticlassDTWClassifier(const std::vector<LabeledPrototype>& prototypes)
{
    auto dim = prototypes[0].Dimension();

    ell::model::Model model;
    auto inputNode = model.AddNode<ell::model::InputNode<double>>(dim);

    std::vector<double> labels = { 0.0 };
    auto threshNode = model.AddNode<ell::nodes::ConstantNode<double>>(5.0);
    ell::model::PortElements<double> dtwOutputs(threshNode->output);

    std::vector<std::pair<int, ell::model::Node*>> dtwNodes;
    for (const auto& prototype : prototypes)
    {
        auto label = prototype.Label();
        labels.push_back(static_cast<double>(label));
        auto dtwNode = model.AddNode<ell::nodes::DTWDistanceNode<double>>(inputNode->output, prototype.Prototype());
        dtwNodes.emplace_back(label, dtwNode);
        dtwOutputs.Append(dtwNode->output);
    }
    auto labelsNode = model.AddNode<ell::nodes::ConstantNode<double>>(labels);
    auto argMinNode = model.AddNode<ell::nodes::ArgMinNode<double>>(dtwOutputs); // val, argVal
    auto selectNode = model.AddNode<ell::nodes::MultiplexerNode<double, int>>(labelsNode->output, argMinNode->argVal);
    model::DynamicMap result(model, { { "input", inputNode } }, { { "output", ell::model::PortElements<double>{ selectNode->output, argMinNode->val } } });
    return result;
}

void TestCompilableMulticlassDTW()
{
    model::Model model;
    std::vector<std::vector<double>> prototype1 = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
    std::vector<std::vector<double>> prototype2 = { { 9, 8, 7 }, { 6, 5, 4 }, { 3, 2, 1 } };
    std::vector<LabeledPrototype> prototypes = { { 3, prototype1 }, { 21, prototype2 } };

    auto map = GenerateMulticlassDTWClassifier(prototypes);

    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "MulticlassDTW");
}

void TestCompilableScalarSumNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto sumNode = model.AddNode<nodes::SumNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", sumNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 4 }, { 3 }, { 2 }, { 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "scalar SumNode");
}

void TestCompilableSumNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto sumNode = model.AddNode<nodes::SumNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", sumNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "SumNode");
}

void TestCompilableUnaryOperationNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto testNode = model.AddNode<nodes::UnaryOperationNode<double>>(inputNode->output, emitters::UnaryOperationType::sqrt);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "UnaryOpNode");
}

void TestCompilableBinaryOperationNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto testNode = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, constantNode->output, emitters::BinaryOperationType::add);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "BinaryOpNode");
}

// Problem: memory corruption for BinaryPredicateNode (probably because of bool foolishness)
void TestCompilableScalarBinaryPredicateNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 2 });
    auto testNode = model.AddNode<nodes::BinaryPredicateNode<double>>(inputNode->output, constantNode->output, emitters::BinaryPredicateType::equal);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1 }, { 4 }, { 7 }, { 2 }, { 4 }, { 1 }, { 11 }, { 24 }, { 92 }, { 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "scalar BinaryPredicateNode");
    // TODO: Fix VerifyCompiledOutput --- types don't match for booleans
    PrintIR(compiledMap);
    PrintCompiledOutput(map, compiledMap, signal, "scalar BinaryPredicateNode");
}

// Problem: memory corruption for BinaryPredicateNode (probably because of bool foolishness)
void TestCompilableBinaryPredicateNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto testNode = model.AddNode<nodes::BinaryPredicateNode<double>>(inputNode->output, constantNode->output, emitters::BinaryPredicateType::equal);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "vector BinaryPredicateNode");
    PrintIR(compiledMap);

    // TODO: Fix VerifyCompiledOutput --- types don't match for booleans
    PrintCompiledOutput(map, compiledMap, signal, "vector BinaryPredicateNode");
}

void TestCompilableMultiplexerNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<int>>(1);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto testNode = model.AddNode<nodes::MultiplexerNode<double, int>>(constantNode->output, inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<int>> signal = { { 0 }, { 1 }, { 0 }, { 1 }, { 1 }, { 0 }, { 0 }, { 1 }, { 1 }, { 0 } };
    VerifyCompiledOutput(map, compiledMap, signal, "MultiplexerNode");
}

void TestCompilableTypeCastNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<int>>(1);
    auto testNode = model.AddNode<nodes::TypeCastNode<int, double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<int>> signal = { { 1 }, { 4 }, { 7 }, { 2 }, { 4 }, { 1 }, { 11 }, { 24 }, { 92 }, { 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "TypeCastNode");
}

//
// Now test nodes that compile themselves as a function
//
void TestCompilableAccumulatorNodeFunction()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto accumNode1 = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);
    auto constNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1, 2, 3 });
    auto accumNode2 = model.AddNode<nodes::AccumulatorNode<double>>(accumNode1->output);
    auto accumNode3 = model.AddNode<nodes::AccumulatorNode<double>>(constNode->output);
    auto dotNode2 = model.AddNode<nodes::DotProductNode<double>>(accumNode2->output, accumNode3->output);
    auto accumNode4 = model.AddNode<nodes::AccumulatorNode<double>>(dotNode2->output);
    auto outputNode = model.AddNode<model::OutputNode<double>>(model::PortElements<double>{ accumNode4->output, dotNode2->output });

    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.unrollLoops = true;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "AccumulatorNodeAsFunction");
}

//
// Now test nodes that compile with callback(s)
//
InputCallbackTester<double> g_tester;
InputCallbackTester<double> g_testerCompiled;

// C callback (called by emitted model)
extern "C" {
bool CompiledSourceNode_InputCallback(double* input)
{
    return g_testerCompiled.InputCallback(input);
}
}

// C++ callback (called by runtime model)
bool SourceNode_InputCallback(std::vector<double>& input)
{
    return g_tester.InputCallback(input);
}

void TestCompilableSourceNode(bool runJit)
{
    const std::vector<std::vector<double>> data = { { 1, 2, 3 }, { 2, 4, 6 }, { 3, 6, 9 }, { 4, 8, 12 }, { 5, 10, 15 } };
    g_tester.Initialize(data);
    g_testerCompiled.Initialize(data);

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<model::TimeTickType>>(2);
    auto testNode = model.AddNode<nodes::SourceNode<double, &SourceNode_InputCallback>>(
        inputNode->output, data[0].size(), "CompiledSourceNode_InputCallback");

    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    if (runJit)
    {
        // compare output
        std::vector<std::vector<model::TimeTickType>> timeSignal = { { 10, 15 }, { 20, 20 }, { 30, 45 }, { 40, 60 }, { 50, 120 } };
        VerifyCompiledOutput(map, compiledMap, timeSignal, "SourceNode");
    }
}

void TestFloatNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<float>>(3);
    auto accumNode1 = model.AddNode<nodes::AccumulatorNode<float>>(inputNode->output);

    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", accumNode1->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.unrollLoops = true;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<float>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "AccumulatorNode<float>");
}

void TestCompilableDotProductNode2(int dimension)
{
    model::Model model;
    std::vector<double> constValue(dimension);
    for (int index = 0; index < dimension; ++index)
    {
        constValue[index] = index + 0.5;
    }
    auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(constValue);
    auto dotNode = model.AddNode<nodes::DotProductNode<double>>(inputNode->output, constantNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", dotNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal;
    for (int index1 = 0; index1 < 8; ++index1)
    {
        std::vector<double> x;
        for (int index2 = 0; index2 < dimension; ++index2)
        {
            x.push_back(index2);
        }
        signal.push_back(x);
    }

    VerifyCompiledOutput(map, compiledMap, signal, "DotProductNode");
}

class BinaryFunctionIRNode : public nodes::IRNode
{
public:
    /// @name Input and Output Ports
    /// @{
    static constexpr const char* input1PortName = "input1";
    static constexpr const char* input2PortName = "input2";
    static constexpr const char* outputPortName = "output";
    const model::InputPort<double>& input1 = _input1;
    const model::InputPort<double>& input2 = _input2;
    const model::OutputPort<double>& output = _output;
    /// @}

    BinaryFunctionIRNode(const model::PortElements<double>& in1, const model::PortElements<double>& in2, const std::string& functionName, const std::string& irCode, const emitters::NamedVariableTypeList& otherArgs)
        : IRNode({ &_input1, &_input2 }, { &_output }, functionName, irCode, otherArgs), _input1(this, in1, input1PortName), _input2(this, in2, input2PortName), _output(this, outputPortName, 1)
    {
    }

protected:
    virtual void Copy(model::ModelTransformer& transformer) const override
    {
        auto newInput1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto newInput2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<BinaryFunctionIRNode>(newInput1, newInput2, GetFunctionName(), GetIRCode(), GetExtraArgs());
        transformer.MapNodeOutput(output, newNode->output);
    }

    virtual std::vector<llvm::Value*> GetNodeFunctionStateArguments(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const override
    {
        int inputSize = _input1.Size();
        assert(inputSize == _input2.Size());
        return { currentFunction.Literal(inputSize) };
    }

private:
    // Inputs
    model::InputPort<double> _input1;
    model::InputPort<double> _input2;

    // Output
    model::OutputPort<double> _output;
};

void TestIRNode()
{
    int dimension = 3;
    std::vector<double> constValue(dimension);
    for (int index = 0; index < dimension; ++index)
    {
        constValue[index] = index + 0.5;
    }

    model::Model dotNodeModel;
    auto inputNode1 = dotNodeModel.AddNode<model::InputNode<double>>(dimension);
    auto constantNode1 = dotNodeModel.AddNode<nodes::ConstantNode<double>>(constValue);
    auto dotNode = dotNodeModel.AddNode<nodes::DotProductNode<double>>(inputNode1->output, constantNode1->output);
    auto dotNodeMap = model::DynamicMap(dotNodeModel, { { "input", inputNode1 } }, { { "output", dotNode->output } });

    model::Model irNodeModel;
    auto inputNode2 = irNodeModel.AddNode<model::InputNode<double>>(dimension);
    auto constantNode2 = irNodeModel.AddNode<nodes::ConstantNode<double>>(constValue);
    std::vector<model::PortElementsBase> inputs{ inputNode2->output, constantNode2->output };
    emitters::NamedVariableTypeList extraArgs{ { "count", emitters::VariableType::Int32 } };

    // Get the precompiled IR for dot product (defined in the DotProductIR.h file)
    auto dotProductIR = GetDotProductIR();
    auto dotProductFunctionName = GetDotProductFunctionName();
    auto irNode = irNodeModel.AddNode<BinaryFunctionIRNode>(inputNode2->output, constantNode2->output, dotProductFunctionName, dotProductIR, extraArgs);
    auto irNodeMap = model::DynamicMap(irNodeModel, { { "input", inputNode2 } }, { { "output", *irNode->GetOutputPort(0) } });

    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(irNodeMap);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal;
    for (int index1 = 0; index1 < 8; ++index1)
    {
        std::vector<double> entry;
        for (int index2 = 0; index2 < dimension; ++index2)
        {
            entry.push_back(index2);
        }
        signal.push_back(entry);
    }

    VerifyCompiledOutput(dotNodeMap, compiledMap, signal, "DotProductNode");
}

void TestBiasLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using LayerType = predictors::neural::BiasLayer<double>;

    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using TensorReferenceType = typename LayerType::TensorReferenceType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;

    // Set up bias layer
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);

    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;

    auto inputPadding = inputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(inputPaddingSize);
    auto outputPadding = outputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(outputPaddingSize);
    Shape outputShape = { { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 } };
    LayerParameters parameters{ inputWithPadding, inputPadding, outputShape, outputPadding };
    VectorType bias({ 10, 100 });

    predictors::neural::BiasLayer<double> biasLayer(parameters, bias);
    biasLayer.Compute();
    auto output = biasLayer.GetOutput();
    testing::ProcessTest("Testing BiasLayer, values",
                         testing::IsEqual(output(outputPaddingSize, outputPaddingSize, 0), 11.0) &&
                             testing::IsEqual(output(outputPaddingSize, outputPaddingSize + 1, 0), 12.0) &&
                             testing::IsEqual(output(outputPaddingSize + 1, outputPaddingSize, 1), 103.0) &&
                             testing::IsEqual(output(outputPaddingSize + 1, outputPaddingSize + 1, 1), 104.0));
    if (outputPaddingSize == 1)
        testing::ProcessTest("Testing BiasLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto biasNode = model.AddNode<nodes::BiasLayerNode<double>>(inputNode->output, biasLayer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", biasNode->output } });

    std::vector<std::vector<double>> signal = { inputWithPadding.ToArray() };
    std::vector<std::vector<double>> expectedOutput = { output.ToArray() };
    VerifyMapOutput(map, signal, expectedOutput, biasNode->GetRuntimeTypeName());

    model::MapCompilerParameters settings;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // compare output
    VerifyCompiledOutput(map, compiledMap, signal, biasNode->GetRuntimeTypeName());
}

void TestBatchNormalizationLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using LayerType = predictors::neural::BatchNormalizationLayer<double>;

    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using TensorReferenceType = typename LayerType::TensorReferenceType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;

    // Verify BatchNormalizationLayer
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input(0, 0, 0) = 11;
    input(0, 1, 0) = 7;
    input(1, 0, 1) = 30;
    input(1, 1, 1) = 50;

    auto inputPadding = inputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(inputPaddingSize);
    auto outputPadding = outputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(outputPaddingSize);
    Shape outputShape = { { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 } };
    LayerParameters bnParameters{ input, inputPadding, outputShape, outputPadding };
    VectorType mean({ 5, 10 });
    VectorType variance({ 4.0, 16.0 });

    predictors::neural::BatchNormalizationLayer<double> bnLayer(bnParameters, mean, variance);
    bnLayer.Compute();
    auto output = bnLayer.GetOutput();
    double eps = 1e-5;
    testing::ProcessTest("Testing BatchNormalizationLayer, values",
                         testing::IsEqual(output(outputPaddingSize, outputPaddingSize, 0), 3.0, eps) &&
                             testing::IsEqual(output(outputPaddingSize, outputPaddingSize + 1, 0), 1.0, eps) &&
                             testing::IsEqual(output(outputPaddingSize + 1, outputPaddingSize, 1), 5.0, eps) &&
                             testing::IsEqual(output(outputPaddingSize + 1, outputPaddingSize + 1, 1), 10.0, eps));
    if (outputPaddingSize == 1)
        testing::ProcessTest("Testing BatchNormalizationLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(input.Size());
    auto bnNode = model.AddNode<nodes::BatchNormalizationLayerNode<double>>(inputNode->output, bnLayer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", bnNode->output } });

    std::vector<std::vector<double>> signal = { inputWithPadding.ToArray() };
    std::vector<std::vector<double>> expectedOutput = { output.ToArray() };
    VerifyMapOutput(map, signal, expectedOutput, bnNode->GetRuntimeTypeName());

    model::MapCompilerParameters settings;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // compare output
    VerifyCompiledOutput(map, compiledMap, signal, bnNode->GetRuntimeTypeName());
}

void TestNeuralNetworkPredictorNode()
{
    using ElementType = double;
    using InputParameters = typename predictors::neural::InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename predictors::neural::Layer<ElementType>::LayerParameters;
    using TensorType = typename predictors::neural::Layer<ElementType>::TensorType;
    using Shape = typename predictors::neural::Layer<ElementType>::Shape;
    using VectorType = typename predictors::neural::Layer<ElementType>::VectorType;
    using MatrixType = typename predictors::neural::Layer<ElementType>::MatrixType;
    using DataVectorType = typename predictors::NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    typename predictors::NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename predictors::NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 1, 1, 3 }, { predictors::neural::PaddingScheme::zeros, 0 }, { 1, 1, 3 }, { predictors::neural::PaddingScheme::zeros, 0 }, 1 };
    inputLayer = std::make_unique<predictors::neural::InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters = { inputLayer->GetOutput(), predictors::neural::NoPadding(), { 1, 1, 3 }, predictors::neural::NoPadding() };
    layerParameters = { inputLayer->GetOutput(), predictors::neural::NoPadding(), { 1, 1, 3 }, predictors::neural::NoPadding() };
    VectorType bias1({ -0.43837756f, -0.90868396f, -0.0323102f });
    layers.push_back(std::unique_ptr<predictors::neural::Layer<ElementType>>(new predictors::neural::BiasLayer<ElementType>(layerParameters, bias1)));
    predictors::NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

    std::vector<ElementType> input = { 0, 1, 2 };
    auto output = neuralNetwork.Predict(DataVectorType(input));

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<double>>(inputNode->output, neuralNetwork);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", predictorNode->output } });

    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName());
}
