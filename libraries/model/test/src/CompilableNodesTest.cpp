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
#include "ActivationLayerNode.h"
#include "BatchNormalizationLayerNode.h"
#include "BiasLayerNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "ConstantNode.h"
#include "DTWDistanceNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "FullyConnectedLayerNode.h"
#include "IRNode.h"
#include "MultiplexerNode.h"
#include "NeuralNetworkPredictorNode.h"
#include "PoolingLayerNode.h"
#include "SinkNode.h"
#include "SoftmaxLayerNode.h"
#include "SourceNode.h"
#include "SumNode.h"
#include "TypeCastNode.h"
#include "UnaryOperationNode.h"

// predictors
#include "NeuralNetworkPredictor.h"

// predictors/neural
#include "ActivationLayer.h"
#include "BatchNormalizationLayer.h"
#include "BiasLayer.h"
#include "BinaryConvolutionalLayer.h"
#include "ConvolutionalLayer.h"
#include "FullyConnectedLayer.h"
#include "InputLayer.h"
#include "LeakyReLUActivation.h"
#include "MaxPoolingFunction.h"
#include "MeanPoolingFunction.h"
#include "PoolingLayer.h"
#include "ReLUActivation.h"
#include "ScalingLayer.h"
#include "SigmoidActivation.h"
#include "SoftmaxLayer.h"

// testing
#include "testing.h"

// common
#include "LoadModel.h" // for RegisterNodeTypes

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
    VerifyCompiledOutput(map, compiledMap, signal, "vector OutputNode");
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
    VerifyCompiledOutput(map, compiledMap, signal, "Multiclass DTW");
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
    VerifyCompiledOutput(map, compiledMap, signal, "UnaryOperationNode");
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
    VerifyCompiledOutput(map, compiledMap, signal, "BinaryOperationNode");
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
    VerifyCompiledOutput(map, compiledMap, signal, "AccumulatorNode as function");
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
TESTING_FORCE_DEFINE_SYMBOL(CompiledSourceNode_InputCallback, bool, double*);

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

// C callback (called by emitted model)
extern "C" {
size_t g_sinkOutputSize = 0;
std::vector<double> outputValues;
void CompiledSinkNode_OutputCallback_Scalar(double output)
{
    assert(g_sinkOutputSize == 1);
    outputValues.push_back(output);
}
TESTING_FORCE_DEFINE_SYMBOL(CompiledSinkNode_OutputCallback_Scalar, void, double);

void CompiledSinkNode_OutputCallback_Vector(double* output)
{
    assert(g_sinkOutputSize > 1);
    outputValues.assign(output, output + g_sinkOutputSize); // assign reallocates as needed
}
}
TESTING_FORCE_DEFINE_SYMBOL(CompiledSinkNode_OutputCallback_Vector, void, double*);

void TestCompilableSinkNode(size_t inputSize, const std::string& sinkFunctionName, bool runJit)
{
    g_sinkOutputSize = inputSize;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputSize);
    auto testNode = model.AddNode<nodes::SinkNode<double>>(inputNode->output, [](const std::vector<double>&) {}, sinkFunctionName);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    if (runJit)
    {
        // compare output
        std::vector<std::vector<double>> input = { {} };
        for (size_t i = 0; i < inputSize; ++i)
        {
            input[0].push_back(i * 10);
        }
        outputValues.clear();
        VerifyCompiledOutput(map, compiledMap, input, "SinkNode");

        // Verify that sink callbacks are actually called
        testing::ProcessTest("Testing callback values", testing::IsEqual(outputValues, input[0]));
    }
}

void TestCompilableSinkNode(bool runJit)
{
    TestCompilableSinkNode(1, "CompiledSinkNode_OutputCallback_Scalar", runJit);
    TestCompilableSinkNode(100, "CompiledSinkNode_OutputCallback_Vector", runJit);
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

//
// Neural network layer nodes
//

// Helper function
template <typename ElementType>
void VerifyLayerMap(const ell::model::DynamicMap& map, const ell::model::Node* computeNode, const typename ell::predictors::neural::Layer<ElementType>::TensorType& inputWithPadding, const typename ell::predictors::neural::Layer<ElementType>::ConstTensorReferenceType& output)
{
    std::vector<std::vector<double>> signal = { inputWithPadding.ToArray() };
    std::vector<std::vector<double>> expectedOutput = { output.ToArray() };
    VerifyMapOutput(map, signal, expectedOutput, computeNode->GetRuntimeTypeName());

    model::MapCompilerParameters settings;
    settings.compilerSettings.useBlas = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // compare output
    VerifyCompiledOutput(map, compiledMap, signal, computeNode->GetRuntimeTypeName());
}

void TestNeuralNetworkPredictorNode1()
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

    InputParameters inputParams{ { 1, 1, 3 }, { predictors::neural::PaddingScheme::zeros, 0 }, { 1, 1, 3 }, { predictors::neural::PaddingScheme::zeros, 0 }, 1 };
    inputLayer = std::make_unique<predictors::neural::InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), predictors::neural::NoPadding(), { 1, 1, 3 }, predictors::neural::NoPadding() };
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
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_1");

    // Test archiving
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);

    archiver << map;
    utilities::JsonUnarchiver unarchiver(strstream, context);
    model::DynamicMap unarchivedMap;
    unarchiver >> unarchivedMap;
}

void TestNeuralNetworkPredictorNode2()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    // Build an XOR net from previously trained values.
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 1, 1, 2 }, { PaddingScheme::zeros, 0 }, { 1, 1, 2 }, { PaddingScheme::zeros, 0 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), NoPadding(), { 1, 1, 3 }, NoPadding() };
    MatrixType weights1(3, 2);
    weights1(0, 0) = -0.97461396f;
    weights1(0, 1) = 1.40845299f;
    weights1(1, 0) = -0.14135513f;
    weights1(1, 1) = -0.54136097f;
    weights1(2, 0) = 0.99313086f;
    weights1(2, 1) = -0.99083692f;
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new FullyConnectedLayer<ElementType>(layerParameters, weights1)));

    layerParameters = { layers[0]->GetOutput(), NoPadding(), { 1, 1, 3 }, NoPadding() };
    VectorType bias1({ -0.43837756f, -0.90868396f, -0.0323102f });
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias1)));

    layerParameters = { layers[1]->GetOutput(), NoPadding(), { 1, 1, 3 }, NoPadding() };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ActivationLayer<ElementType, ReLUActivation>(layerParameters)));

    layerParameters = { layers[2]->GetOutput(), NoPadding(), { 1, 1, 1 }, NoPadding() };
    MatrixType weights2(1, 3);
    weights2(0, 0) = 1.03084767f;
    weights2(0, 1) = -0.10772263f;
    weights2(0, 2) = 1.04077697f;
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new FullyConnectedLayer<ElementType>(layerParameters, weights2)));

    layerParameters = { layers[3]->GetOutput(), NoPadding(), { 1, 1, 1 }, NoPadding() };
    VectorType bias2({ 1.40129846e-20f });
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias2)));

    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    std::vector<ElementType> input = { 0, 1 };
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
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_2");
}

template <typename ElementType>
void FillTensor(ell::math::ChannelColumnRowTensor<ElementType>& tensor, int startValue = 0)
{
    int val = 0;
    tensor.Generate([&val]() { return val++; });
}

template <typename ElementType>
void FillVector(ell::math::ColumnVector<ElementType>& vector, int startValue = 0)
{
    int val = 0;
    vector.Generate([&val]() { return val++; });
}

void TestNeuralNetworkPredictorNode3()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 3, 3, 3 }, { PaddingScheme::zeros, 0 }, { 5, 5, 3 }, { PaddingScheme::zeros, 1 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), { PaddingScheme::zeros, 1 }, { 3, 3, 8 }, NoPadding() };
    auto convolutionMethod = ConvolutionMethod::columnwise;
    ConvolutionalParameters convolutionalParams{ 3, 1, convolutionMethod, 1 };
    TensorType convWeights1(8 * 3, 3, 3);
    FillTensor(convWeights1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights1)));

    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    std::vector<ElementType> input(3 * 3 * 3);
    int val = 0;
    std::generate(input.begin(), input.end(), [&val]() { return val++; });

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<double>>(inputNode->output, neuralNetwork);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", predictorNode->output } });

    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_3");
}

void TestNeuralNetworkPredictorNode4()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    // Input Layer
    InputParameters inputParams = { { 3, 3, 3 }, { PaddingScheme::zeros, 0 }, { 5, 5, 3 }, { PaddingScheme::zeros, 1 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    // ConvolutionalLayer
    LayerParameters layerParameters{ inputLayer->GetOutput(), { PaddingScheme::zeros, 1 }, { 3, 3, 8 }, NoPadding() };
    auto convolutionMethod = ConvolutionMethod::columnwise;
    ConvolutionalParameters convolutionalParams{ 3, 1, convolutionMethod, 1 };
    TensorType convWeights1(8 * 3, 3, 3);
    FillTensor(convWeights1, -10);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights1)));

    // BiasLayer
    layerParameters = { layers[0]->GetOutput(), NoPadding(), { 3, 3, 8 }, NoPadding() };
    VectorType bias1(layerParameters.outputShape[2]);
    FillVector(bias1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias1)));

    // ActivationLayer
    layerParameters = { layers[1]->GetOutput(), NoPadding(), { 3, 3, 8 }, NoPadding() };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ActivationLayer<ElementType, ReLUActivation>(layerParameters)));

    // BatchNormalizationLayer
    layerParameters = { layers[2]->GetOutput(), NoPadding(), { 3, 3, 8 }, NoPadding() };
    VectorType mean(layerParameters.outputShape[2]);
    VectorType variance(layerParameters.outputShape[2]);
    FillVector(mean);
    FillVector(variance);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BatchNormalizationLayer<ElementType>(layerParameters, mean, variance, 1.0e-6f, EpsilonSummand::SqrtVariance)));

    // ScalingLayer
    layerParameters = { layers[3]->GetOutput(), NoPadding(), { 5, 5, 8 }, { PaddingScheme::zeros, 1 } };
    VectorType scales(layerParameters.outputShape[2]);
    FillVector(scales, -3);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ScalingLayer<ElementType>(layerParameters, scales)));

    // Max PoolingLayer
    layerParameters = { layers[4]->GetOutput(), { PaddingScheme::zeros, 1 }, { 2, 2, 8 }, NoPadding() };
    PoolingParameters poolingParameters{ 2, 1 };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new PoolingLayer<ElementType, MaxPoolingFunction>(layerParameters, poolingParameters)));

    // Mean PoolingLayer
    layerParameters = { layers[5]->GetOutput(), NoPadding(), { 1, 1, 8 }, NoPadding() };
    PoolingParameters poolingParameters2{ 2, 2 };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new PoolingLayer<ElementType, MeanPoolingFunction>(layerParameters, poolingParameters2)));

    // Create the predictor
    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    std::vector<ElementType> input(3 * 3 * 3);
    int val = 0;
    std::generate(input.begin(), input.end(), [&val]() { return val++; });

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<double>>(inputNode->output, neuralNetwork);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", predictorNode->output } });

    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_4");
}

void TestInputLayerNode(size_t outputPadding)
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

    // Input layer
    InputParameters inputParams{ { 1, 1, 3 }, predictors::neural::NoPadding(), { 2 * outputPadding + 1, 2 * outputPadding + 1, 3 }, predictors::neural::ZeroPadding(outputPadding), 1.0 };
    inputLayer = std::make_unique<predictors::neural::InputLayer<ElementType>>(inputParams);

    // Pooling layer
    auto&& x = inputLayer->GetOutputMinusPadding();
    const size_t poolingSize = 3;
    const size_t poolingStride = 1;

    LayerParameters layerParameters{ inputLayer->GetOutput(), predictors::neural::ZeroPadding(outputPadding), { 1, 1, 3 }, predictors::neural::NoPadding() };
    predictors::neural::PoolingParameters poolingParameters{ poolingSize, poolingStride };
    layers.push_back(std::unique_ptr<predictors::neural::Layer<ElementType>>(new predictors::neural::PoolingLayer<ElementType, predictors::neural::MaxPoolingFunction>(layerParameters, poolingParameters)));
    predictors::NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

    std::vector<ElementType> input = { 0, 1, 2 };
    auto output = neuralNetwork.Predict(DataVectorType(input));

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, neuralNetwork);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", predictorNode->output } });

    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<ElementType>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, "InputLayer");
}

template <template <typename> class ActivationFunction>
void TestActivationLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;
    const double eps = 1e-6;

    // Build a model
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input(0, 0, 0) = 1.0;
    input(0, 1, 0) = -2.0;
    input(1, 0, 1) = 3.0;
    input(1, 1, 1) = -4.0;
    Shape outputShape = { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };
    LayerParameters layerParameters{ input, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };

    ActivationLayer<ElementType, ActivationFunction> layer(layerParameters);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::ActivationLayerNode<double, ActivationFunction>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestReLUActivationLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    TestActivationLayerNode<ell::predictors::neural::ReLUActivation>(inputPaddingSize, outputPaddingSize);
}

void TestLeakyReLUActivationLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    TestActivationLayerNode<ell::predictors::neural::LeakyReLUActivation>(inputPaddingSize, outputPaddingSize);
}

void TestSigmoidActivationLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    TestActivationLayerNode<ell::predictors::neural::SigmoidActivation>(inputPaddingSize, outputPaddingSize);
}

void TestBatchNormalizationLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using ElementType = double;
    using LayerType = predictors::neural::BatchNormalizationLayer<double>;
    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using TensorReferenceType = typename LayerType::TensorReferenceType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;

    // Build a model
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input(0, 0, 0) = 11;
    input(0, 1, 0) = 7;
    input(1, 0, 1) = 30;
    input(1, 1, 1) = 50;

    auto inputPadding = inputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(inputPaddingSize);
    auto outputPadding = outputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(outputPaddingSize);
    Shape outputShape = { { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 } };
    LayerParameters layerParameters{ input, inputPadding, outputShape, outputPadding };
    VectorType mean({ 5, 10 });
    VectorType variance({ 4.0, 16.0 });

    predictors::neural::BatchNormalizationLayer<double> layer(layerParameters, mean, variance, 1.0e-6f, predictors::neural::EpsilonSummand::SqrtVariance);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::BatchNormalizationLayerNode<double>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestBiasLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using ElementType = double;
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

    predictors::neural::BiasLayer<double> layer(parameters, bias);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::BiasLayerNode<double>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestBinaryConvolutionalLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    // Verify BinaryConvolutionalLayer with bitwise method
    TensorType inputWithPadding(1 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input.Fill(0);
    input(0, 0, 0) = 2;
    input(0, 1, 0) = 1;
    input(0, 0, 1) = 3;
    input(0, 1, 1) = 2;
    Shape outputShape = { 1 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };
    LayerParameters parameters{ inputWithPadding, MinusOnePadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::bitwise };
    TensorType weights(convolutionalParams.receptiveField * outputShape[2], convolutionalParams.receptiveField, input.NumChannels());
    // clang-format off
    // Weights size: f x k x k x d = 2*3*3*2 = 36
    std::vector<ElementType> weightsVector{   // RowMajor then depth order
        1, -3, 2, -3, 1, -1, 2, 3, -1,
        2, 4, -1, 3, -1, 2, -1, 4, 2,
        1, 2, 1, -2, 3, -2, 1, -2, 1,
        0, 3, 2, 3, -1, 2, -1, 0, -2 };
    // clang-format on
    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape[2]; f++)
    {
        for (size_t k = 0; k < input.NumChannels(); k++)
        {
            for (size_t i = 0; i < convolutionalParams.receptiveField; i++)
            {
                for (size_t j = 0; j < convolutionalParams.receptiveField; j++)
                {
                    weights(f * convolutionalParams.receptiveField + i, j, k) = weightsVector[vectorIndex++];
                }
            }
        }
    }

    BinaryConvolutionalLayer<ElementType> layer(parameters, convolutionalParams, weights);
    layer.Compute();
    auto output = layer.GetOutput();

    // testing::ProcessTest("Testing BinaryConvolutionalLayer (bitwise), values", Equals(output(0, 0, 0), -20.5555553) && Equals(output(0, 0, 1), -9.66666603) && Equals(output(0, 1, 0), -20.5555553) && Equals(output(0, 1, 1), -9.66666603));
    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::BinaryConvolutionalLayerNode<double>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestBinaryConvolutionalLayerNode2(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    const size_t numRows = 4;
    const size_t numCols = 4;
    const size_t numChannels = 2;
    const size_t numFilters = 2;

    assert(inputPaddingSize == 1);
    TensorType inputWithPadding(numRows + 2 * inputPaddingSize, numCols + 2 * inputPaddingSize, numChannels);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, numRows, numCols, numChannels);
    inputWithPadding.Fill(0);
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (size_t colIndex = 0; colIndex < numCols; ++colIndex)
        {
            for (size_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                input(rowIndex, colIndex, channelIndex) = 1.25 * (rowIndex - (numRows/2)) + 0.75 * (colIndex-(numCols/2)) + (.0125 * channelIndex);
            }
        }
    }
    Shape outputShape = { numRows + 2 * outputPaddingSize, numCols + 2 * outputPaddingSize, numFilters };

    LayerParameters parameters{ inputWithPadding, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::bitwise };
    TensorType weights(convolutionalParams.receptiveField * numFilters, convolutionalParams.receptiveField, input.NumChannels());
    weights.Fill(1.0);
    for (size_t rowIndex = 0; rowIndex < convolutionalParams.receptiveField * numFilters; ++rowIndex)
    {
        for (size_t colIndex = 0; colIndex < convolutionalParams.receptiveField; ++colIndex)
        {
            for (size_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                weights(rowIndex, colIndex, channelIndex) = 1.5 * rowIndex + 3.3 * colIndex + 0.15 * channelIndex;
            }
        }
    }

    //
    // Verify BinaryConvolutionalLayerNode
    //
    BinaryConvolutionalLayer<ElementType> layer(parameters, convolutionalParams, weights);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::BinaryConvolutionalLayerNode<double>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestConvolutionalLayerNode(ConvolutionType convolutionType, size_t inputPaddingSize, size_t outputPaddingSize)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    assert(inputPaddingSize == 1);
    TensorType inputWithPadding(1 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    inputWithPadding.Fill(0);
    input(0, 0, 0) = 2;
    input(0, 1, 0) = 1;
    input(0, 0, 1) = 3;
    input(0, 1, 1) = 2;
    Shape outputShape = { 1 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };

    LayerParameters parameters{ inputWithPadding, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    auto convolutionMethod = (convolutionType == ConvolutionType::Diagonal) ? ConvolutionMethod::diagonal : ConvolutionMethod::columnwise;
    ConvolutionalParameters convolutionalParams{ 3, 1, convolutionMethod, 2 }; // 2 == batch size
    TensorType weights(convolutionalParams.receptiveField * outputShape[2], convolutionalParams.receptiveField, input.NumChannels());
    // clang-format off
    std::vector<ElementType> weightsVector{   // RowMajor then depth order
        1, 3, 2, 3, 1, 1, 2, 3, 1,
        2, 4, 1, 3, 1, 2, 1, 4, 2,
        1, 2, 1, 2, 3, 2, 1, 2, 1,
        0, 3, 2, 3, 1, 2, 1, 0, 2 };
    // clang-format on
    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape[2]; f++)
    {
        for (size_t k = 0; k < input.NumChannels(); k++)
        {
            for (size_t i = 0; i < convolutionalParams.receptiveField; i++)
            {
                for (size_t j = 0; j < convolutionalParams.receptiveField; j++)
                {
                    weights(f * convolutionalParams.receptiveField + i, j, k) = weightsVector[vectorIndex++];
                }
            }
        }
    }

    //
    // Verify ConvolutionalLayerNode
    //
    ConvolutionalLayer<ElementType> layer(parameters, convolutionalParams, weights);
    layer.Compute();
    auto output = layer.GetOutput();
    ElementType eps = 1e-6;
    ElementType v1 = 10;
    ElementType v2 = 15;
    ElementType v3 = 18;
    ElementType v4 = 18;
    testing::ProcessTest("Testing ConvolutionalLayer, values",
                         testing::IsEqual(output(0, 0, 0), v1, eps) &&
                             testing::IsEqual(output(0, 0, 1), v2, eps) &&
                             testing::IsEqual(output(0, 1, 0), v3, eps) &&
                             testing::IsEqual(output(0, 1, 1), v4, eps));

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::ConvolutionalLayerNode<double>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestConvolutionalLayerNode2(ConvolutionType convolutionType, size_t inputPaddingSize, size_t outputPaddingSize)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    const size_t numRows = 56;
    const size_t numCols = 56;
    const size_t numChannels = 16;
    const size_t numFilters = 128;

    assert(inputPaddingSize == 1);
    TensorType inputWithPadding(numRows + 2 * inputPaddingSize, numCols + 2 * inputPaddingSize, numChannels);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, numRows, numCols, numChannels);
    inputWithPadding.Fill(0);
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (size_t colIndex = 0; colIndex < numCols; ++colIndex)
        {
            for (size_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                input(rowIndex, colIndex, channelIndex) = 1.25 * rowIndex + 0.75 * colIndex + channelIndex;
            }
        }
    }
    Shape outputShape = { numRows + 2 * outputPaddingSize, numCols + 2 * outputPaddingSize, numFilters };

    LayerParameters parameters{ inputWithPadding, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    auto convolutionMethod = (convolutionType == ConvolutionType::Diagonal) ? ConvolutionMethod::diagonal : ConvolutionMethod::columnwise;
    ConvolutionalParameters convolutionalParams{ 3, 1, convolutionMethod, 2 }; // 2 == batch size
    TensorType weights(convolutionalParams.receptiveField * numFilters, convolutionalParams.receptiveField, input.NumChannels());
    weights.Fill(1.0);
    for (size_t rowIndex = 0; rowIndex < convolutionalParams.receptiveField * numFilters; ++rowIndex)
    {
        for (size_t colIndex = 0; colIndex < convolutionalParams.receptiveField; ++colIndex)
        {
            for (size_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                weights(rowIndex, colIndex, channelIndex) = 1.5 * rowIndex + 3.3 * colIndex + 0.15 * channelIndex;
            }
        }
    }

    //
    // Verify ConvolutionalLayerNode
    //
    ConvolutionalLayer<ElementType> layer(parameters, convolutionalParams, weights);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::ConvolutionalLayerNode<double>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestFullyConnectedLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using ElementType = double;
    using LayerType = predictors::neural::FullyConnectedLayer<double>;
    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using TensorReferenceType = typename LayerType::TensorReferenceType;
    using MatrixType = typename LayerType::MatrixType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;

    // Set up layer
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;

    auto inputPadding = inputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(inputPaddingSize);
    auto outputPadding = outputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(outputPaddingSize);
    Shape outputShape = { { 4 + 2 * outputPaddingSize, 1 + 2 * outputPaddingSize, 1 } };
    LayerParameters parameters{ input, inputPadding, outputShape, outputPadding };
    MatrixType weights(4, 8);
    for (int index = 0; index < 8; index++)
        weights(1, index) = static_cast<double>(index);
    for (int index = 0; index < 8; index++)
        weights(2, index) = static_cast<double>(7 - index);
    for (int index = 0; index < 8; index++)
        weights(3, index) = 1.0;

    LayerType layer(parameters, weights);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::FullyConnectedLayerNode<double>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

template <template <typename> class PoolingFunction>
void TestPoolingLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;
    const double eps = 1e-6;

    // Build a model
    const size_t inRows = 8;
    const size_t inCols = 8;
    const size_t outRows = 6;
    const size_t outCols = 6;
    const size_t numDims = 2;
    const size_t poolingSize = 3;
    const size_t poolingStride = 1;

    TensorType inputWithPadding(inRows + 2 * inputPaddingSize, inCols + 2 * inputPaddingSize, numDims);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, inRows, inCols, numDims);
    input(1, 1, 0) = 1.0;
    input(1, 2, 0) = 2.0;
    input(2, 1, 1) = 3.0;
    input(2, 2, 1) = 4.0;

    Shape outputShape = { outRows + 2 * outputPaddingSize, outCols + 2 * outputPaddingSize, numDims };
    LayerParameters layerParameters{ inputWithPadding, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    PoolingParameters poolingParameters{ poolingSize, poolingStride };
    PoolingLayer<ElementType, PoolingFunction> layer(layerParameters, poolingParameters);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::PoolingLayerNode<double, PoolingFunction>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestMaxPoolingLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    TestPoolingLayerNode<predictors::neural::MaxPoolingFunction>(inputPaddingSize, outputPaddingSize);
}

void TestMeanPoolingLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    TestPoolingLayerNode<ell::predictors::neural::MeanPoolingFunction>(inputPaddingSize, outputPaddingSize);
}

void TestScalingLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using LayerType = predictors::neural::ScalingLayer<ElementType>;
    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using TensorReferenceType = typename LayerType::TensorReferenceType;
    using MatrixType = typename LayerType::MatrixType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;
    const ElementType eps = 1e-6;

    // Set up scaling layer
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;
    Shape outputShape = { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };
    LayerParameters layerParameters{ input, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    VectorType scale({ 5, 10 });

    LayerType layer(layerParameters, scale);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::ScalingLayerNode<ElementType>>(inputNode->output, layer);

    inputNode->SetInput(input.ToArray());
    auto modelOutput = model.ComputeOutput(computeNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });
    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestSoftmaxLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using LayerType = predictors::neural::SoftmaxLayer<ElementType>;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;
    const ElementType eps = 1e-6;

    // Build a model
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input(0, 0, 0) = 1.0;
    input(0, 1, 0) = -2.0;
    input(1, 0, 1) = 3.0;
    input(1, 1, 1) = -4.0;
    Shape outputShape = { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };
    LayerParameters layerParameters{ input, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    LayerType layer(layerParameters);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::SoftmaxLayerNode<ElementType>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });
    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}
