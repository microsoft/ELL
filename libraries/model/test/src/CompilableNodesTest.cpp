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

// common
#include "LoadModel.h" // for RegisterNodeTypes

// math
#include "Tensor.h"

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
#include "PortMemoryLayout.h"

// nodes
#include "AccumulatorNode.h"
#include "ActivationLayerNode.h"
#include "BatchNormalizationLayerNode.h"
#include "BiasLayerNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "ClockNode.h"
#include "ConstantNode.h"
#include "DTWDistanceNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "FullyConnectedLayerNode.h"
#include "GRULayerNode.h"
#include "IRNode.h"
#include "L2NormSquaredNode.h"
#include "LSTMLayerNode.h"
#include "MatrixMatrixMultiplyNode.h"
#include "MatrixVectorMultiplyNode.h"
#include "MatrixVectorProductNode.h"
#include "MultiplexerNode.h"
#include "NeuralNetworkPredictorNode.h"
#include "PoolingLayerNode.h"
#include "ReceptiveFieldMatrixNode.h"
#include "RecurrentLayerNode.h"
#include "ReorderDataNode.h"
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
#include "ParametricReLUActivation.h"
#include "PoolingLayer.h"
#include "ReLUActivation.h"
#include "ScalingLayer.h"
#include "SigmoidActivation.h"
#include "SoftmaxLayer.h"

// testing
#include "testing.h"

// utilities
#include "RandomEngines.h"
#include "Logger.h"

// stl
#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

using namespace ell;
using namespace ell::predictors;
using namespace ell::predictors::neural;
using namespace ell::logging;

namespace
{
size_t GetShapeSize(const math::IntegerTriplet& shape)
{
    return shape[0] * shape[1] * shape[2];
}

template <typename ValueType>
class Uniform
{
public:
    Uniform(ValueType minVal, ValueType maxVal, std::string seed = "123")
        : _rng(utilities::GetRandomEngine(seed)), _range(static_cast<double>(_rng.max() - _rng.min())), _minOutput(minVal), _outputRange(maxVal - minVal) {}

    ValueType operator()()
    {
        double uniform = static_cast<double>(_rng()) / _range;
        return static_cast<ValueType>((uniform * _outputRange) + _minOutput);
    }

private:
    std::default_random_engine _rng;
    double _range;
    ValueType _minOutput;
    ValueType _outputRange;
};

template <typename ElementType>
void FillRandomVector(std::vector<ElementType>& vector, ElementType min = -1, ElementType max = 1)
{
    Uniform<ElementType> rand(min, max);
    std::generate(vector.begin(), vector.end(), rand);
}

template <typename ElementType>
std::vector<ElementType> GetRandomVector(size_t size, ElementType min = -1, ElementType max = 1)
{
    std::vector<ElementType> result(size);
    FillRandomVector(result, min, max);
    return result;
}

template <typename ElementType>
void FillRandomVector(ell::math::ColumnVector<ElementType>& vector, ElementType min = -1, ElementType max = 1)
{
    Uniform<ElementType> rand(min, max);
    vector.Generate(rand);
}

template <typename ElementType>
void FillRandomTensor(ell::math::ChannelColumnRowTensor<ElementType>& tensor, ElementType min = -1, ElementType max = 1)
{
    Uniform<ElementType> rand(min, max);
    tensor.Generate(rand);
}

template <typename ElementType>
void FillVector(std::vector<ElementType>& vector, ElementType startValue = 0, ElementType step = 1)
{
    ElementType val = startValue;
    std::generate(vector.begin(), vector.end(), [&val, step]() {
        auto result = val;
        val += step;
        return result; });
}

template <typename ElementType>
void FillVector(ell::math::ColumnVector<ElementType>& vector, ElementType startValue = 0, ElementType step = 1)
{
    ElementType val = startValue;
    vector.Generate([&val]() { return val++; });
}

template <typename ElementType>
void FillTensor(ell::math::ChannelColumnRowTensor<ElementType>& tensor, ElementType startValue = 0, ElementType step = 1)
{
    ElementType val = startValue;
    tensor.Generate([&val, step]() {
        auto result = val;
        val += step;
        return result; });
}

template <typename ElementType>
void FillTensor(math::TensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>& tensor, ElementType startValue = 0, ElementType step = 1)
{
    ElementType val = startValue;
    tensor.Generate([&val, step]() {
        auto result = val;
        val += step;
        return result; });
}

template <typename ElementType>
void FillWeightsTensor(ell::math::ChannelColumnRowTensor<ElementType>& tensor, ElementType startValue = 0, ElementType step = 1)
{
    ElementType val = startValue;
    tensor.Generate([&val, step]() {
        auto result = val;
        val += step;
        return result; });
}
} // end anonymous namespace

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
    using ElementType = int64_t;
    std::vector<std::vector<ElementType>> signal = { { 1, 2, 3, 4, 5, 6 }, { 7, 8, 9, 3, 4, 5 }, { 2, 3, 2, 1, 5, 3 }, { 1, 2, 3, 4, 5, 6 }, { 7, 8, 9, 7, 4, 2 }, { 5, 2, 1, 2, 5, 9 } };

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(signal[0].size());
    auto sumNode = model.AddNode<nodes::SumNode<ElementType>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", sumNode->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.allowVectorInstructions = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare output
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

void TestCompilableUnaryOperation_square_Node()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto testNode = model.AddNode<nodes::UnaryOperationNode<double>>(inputNode->output, emitters::UnaryOperationType::square);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    PrintIR(compiledMap);
    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "UnaryOperationNode_square");
}

void TestL2NormSquaredNodeCompiled()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto testNode = model.AddNode<nodes::L2NormSquaredNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    PrintIR(compiledMap);
    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "L2NormSquaredNode");
}

void TestMatrixVectorProductNodeCompile()
{
    math::RowMatrix<double> m{
        {1.2, 1.1, 0.8},
        {0.6, 0.9, 1.3},
        {0.3, 1.0, 0.4},
        {-.4, 0.2, -.7}
    };
    m.Transform([](double d) { return -2.0 * d; });

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto testNode = model.AddNode<nodes::MatrixVectorProductNode<double, math::MatrixLayout::rowMajor>>(inputNode->output, m);
    auto outputNode = model.AddNode<model::OutputNode<double>>(testNode->output, model::OutputShape{1, 4, 1});
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = false;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    PrintIR(compiledMap);
    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "MatrixVectorProductNode");
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

void TestCompilableTypeCastNode(size_t dimension)
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<int>>(dimension);
    auto testNode = model.AddNode<nodes::TypeCastNode<int, double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    // compare output
    const int numEntries = 10;
    std::vector<std::vector<int>> signal;
    std::generate_n(std::back_inserter(signal), numEntries, [dimension]{ return GetRandomVector<int>(dimension, 0, 100); });
    VerifyCompiledOutput(map, compiledMap, signal, "TypeCastNode");
}

void TestReorderDataNode1()
{
    using ElementType = float;
    int numRows = 3;
    int numColumns = 3;
    int numChannels = 2;
    model::Model model;
    model::PortMemoryLayout inputShape({ numRows, numColumns, numChannels }); // Default order: 0, 1, 2 == rows, columns, channels
    model::PortMemoryLayout outputShape({ numChannels, numRows, numColumns }); // Want to reorder to 2, 0, 1 == channels, rows, columns

    //        [  (1,2)   (3,4)    (5,6) ]
    // Input: [  (7,8)  (9,10), (11,12))]
    //        [(13,14) (15,16)  (17,18) ]
    //
    // = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
    //
    //         [  1  3  5 ]   [  2  4  6 ]
    // Output: [  7  9 11 ]   [  8 10 12 ]
    //         [ 13 15 17 ]   [ 14 16 18 ]
    //
    // = 1 3 5 7 9 11 13 15 17 2 4 6 8 10 12 14 16 18

    size_t inputSize = inputShape.GetMemorySize();
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputSize);
    // auto testNode = model.AddNode<nodes::ReorderDataNode<ElementType>>(inputNode->output, inputShape, outputShape);
    auto testNode = model.AddNode<nodes::ReorderDataNode<ElementType>>(inputNode->output, inputShape, outputShape, std::vector<int>{ 2, 0, 1 });
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    std::vector<ElementType> input(inputSize);
    FillVector(input, 1.0f);
    Log() << "Input:" << EOL << input << EOL;

    // compare output
    std::vector<std::vector<ElementType>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, "ReorderDataNode");
}

void TestReorderDataNode2()
{
    using ElementType = float;
    int numRows = 3;
    int numColumns = 3;
    int numChannels = 2;
    int padding = 1;
    model::Model model;
    model::PortMemoryLayout inputShape({ numRows, numColumns, numChannels }, { padding, padding, 0 }); // Default order: 0, 1, 2 == rows, columns, channels
    model::PortMemoryLayout outputShape({ numRows, numColumns, numChannels }); // Transform to order (channels, rows, cols) and remove padding

    //        [    (1,2)   (3,4)   (5,6)   (7,8)   9,10) ]
    // Input: [  (11,12) (13,14) (15,16) (17,18) (19,20) ]
    //        [  (21,22) (23,24) (25,26) (27,28) (29,30) ]
    //        [  (31,32) (33,34) (35,36) (37,38) (39,40) ]
    //        [  (41,42) (43,44) (45,46) (47,48) (49,50) ]
    //
    // = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 ... 50
    //
    //         [ 13 15 17 ]   [ 14 16 18 ]
    // Output: [ 23 25 27 ]   [ 24 26 28 ]
    //         [ 33 35 37 ]   [ 34 36 38 ]
    //
    // = 13 15 17 23 25 27 33 35 37 14 16 18 24 26 28 34 36 38

    size_t inputSize = inputShape.GetMemorySize();
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputSize);
    auto testNode = model.AddNode<nodes::ReorderDataNode<ElementType>>(inputNode->output, inputShape, outputShape, std::vector<int>{ 2, 0, 1 });
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    std::vector<ElementType> input(inputSize);
    FillVector(input, 1.0f);
    std::cout << "Input:\n"
              << input << std::endl;

    // compare output
    std::vector<std::vector<ElementType>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, "ReorderDataNode");
}

void TestReceptiveFieldMatrixNode(size_t numChannels, bool useNewReshape)
{
    const std::array<int, 3> rcdOrder = std::array<int, 3>{ 0, 1, 2 };
    const std::array<int, 3> drcOrder = std::array<int, 3>{ 2, 0, 1 };
    std::array<int, 3> dataOrder = useNewReshape ? drcOrder : rcdOrder;

    using ElementType = float;
    int numRows = 3;
    int numColumns = 3;

    model::PortMemoryLayout inputMemoryLayout({ numRows, numColumns, (int)numChannels });
    size_t filterWidth = 3;
    size_t stride = 1;
    size_t paddingSize = 1;
    size_t outputHeight = numRows;
    size_t outputWidth = numColumns;

    //
    // 1 input channel:
    //
    //        [ 1 2 3 ]
    // Input: [ 4 5 6 ]
    //        [ 7 8 9 ]
    //
    // = 1 2 3 4 5 6 7 8 9
    //
    //         [ 0 0 0  0 1 2  0 4 5 ]
    //         [ 0 0 0  1 2 3  4 5 6 ]
    //         [ 0 0 0  2 3 0  5 6 0 ]
    //         [ 0 1 2  0 4 5  0 7 8 ]
    // Output: [ 1 2 3  4 5 6  7 8 9 ]
    //         [ 2 3 0  5 6 0  8 9 0 ]
    //         [ 0 4 5  0 7 8  0 0 0 ]
    //         [ 4 5 6  7 8 9  0 0 0 ]
    //         [ 5 6 0  8 9 0  0 0 0 ]

    //
    // 2 input channels:
    //
    //        [ 1 2 3 ]  [ 10 11 12 ]
    // Input: [ 4 5 6 ]  [ 13 14 15 ]
    //        [ 7 8 9 ]  [ 16 17 18 ]
    //
    // = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
    //
    //         [ 0 0 0  0 1 2  0 4 5 ]
    //         [ 0 0 0  0 10 11  0 13 14 ]
    //         [ 0 0 0  1 2 3  4 5 6 ]
    //         [ 0 0 0  10 11 12  13 14 15 ]
    //         [ 0 0 0  2 3 0  5 6 0 ]
    //         [ 0 0 0  11 12 0  14 15 0 ]
    //
    //         [ 0 1 2  0 4 5  0 7 8 ]
    //         [ 0 10 11  0 13 14  0 16 17 ]
    // Output: [ 1 2 3  4 5 6  7 8 9 ]
    //         [ 10 11 12  13 14 15  16 17 18 ]
    //         [ 2 3 0  5 6 0  8 9 0 ]
    //         [ 11 12 0  14 15 0  17 18 0]
    //
    //         [ 0 4 5  0 7 8  0 0 0 ]
    //         [ 0 13 14  0 16 17  0 0 0 ]
    //         [ 4 5 6  7 8 9  0 0 0 ]
    //         [ 13 14 15  16 17 18  0 0 0 ]
    //         [ 5 6 0  8 9 0  0 0 0 ]
    //         [ 14 15 0  17 18 0  0 0 0 ]
    //

    size_t inputSize = model::NumElements(inputMemoryLayout.GetStride());
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputSize);
    auto testNode = model.AddNode<nodes::ReceptiveFieldMatrixNode<ElementType>>(inputNode->output,
                                                                                inputMemoryLayout,
                                                                                filterWidth,
                                                                                stride,
                                                                                paddingSize,
                                                                                dataOrder,
                                                                                outputWidth,
                                                                                outputHeight);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    std::vector<ElementType> input(inputSize, 1.0);
    FillVector(input, 1.0f);
    compiledMap.SetInputValue(0, input);
    auto compiledResult = compiledMap.ComputeOutput<ElementType>(0);
    std::cout << "Compiled result size: " << compiledResult.size() << std::endl;
    std::cout << "  " << compiledResult << std::endl;

    if (numChannels == 1)
    {
        // Test vs. known output
        std::vector<ElementType> desiredOutput = { 0, 0, 0, 0, 1, 2, 0, 4, 5, 0, 0, 0, 1, 2, 3, 4, 5, 6, 0, 0, 0, 2, 3, 0, 5, 6, 0, 0, 1, 2, 0, 4, 5, 0, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8, 9, 2, 3, 0, 5, 6, 0, 8, 9, 0, 0, 4, 5, 0, 7, 8, 0, 0, 0, 4, 5, 6, 7, 8, 9, 0, 0, 0, 5, 6, 0, 8, 9, 0, 0, 0, 0 };
        testing::ProcessTest("Testing compiled ReceptiveFieldMatrixNode", testing::IsEqual(compiledResult, desiredOutput));
    }
    else if (numChannels == 2)
    {
        // Test vs. known output
        std::vector<ElementType> desiredOutput = { 0, 0, 0, 0, 1, 2, 0, 4, 5, 0, 0, 0, 0, 10, 11, 0, 13, 14, 0, 0, 0, 1, 2, 3, 4, 5, 6, 0, 0, 0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 2, 3, 0, 5, 6, 0, 0, 0, 0, 11, 12, 0, 14, 15, 0, 0, 1, 2, 0, 4, 5, 0, 7, 8, 0, 10, 11, 0, 13, 14, 0, 16, 17, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 2, 3, 0, 5, 6, 0, 8, 9, 0, 11, 12, 0, 14, 15, 0, 17, 18, 0, 0, 4, 5, 0, 7, 8, 0, 0, 0, 0, 13, 14, 0, 16, 17, 0, 0, 0, 4, 5, 6, 7, 8, 9, 0, 0, 0, 13, 14, 15, 16, 17, 18, 0, 0, 0, 5, 6, 0, 8, 9, 0, 0, 0, 0, 14, 15, 0, 17, 18, 0, 0, 0, 0 };
        testing::ProcessTest("Testing compiled ReceptiveFieldMatrixNode", testing::IsEqual(compiledResult, desiredOutput));
    }

    // compare compiled version with computed version (currently disabled because Compute() isn't implemented)
    // std::vector<std::vector<ElementType>> signal = { input };
    // VerifyCompiledOutput(map, compiledMap, signal, "ReceptiveFieldMatrixNode");
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

// C callback (called by emitted code)
extern "C"
{
size_t g_callbackCount = 0;
const size_t g_inputSize = 5;
bool Test_CompiledSourceNode_InputCallback(double* input)
{
    Log() << "Source Input Callback " << input << EOL;
    for (size_t i = 0; i < g_inputSize; ++i)
    {
        input[i] = 42.0;
    }
    g_callbackCount++;
    return true;
}
TESTING_FORCE_DEFINE_SYMBOL(Test_CompiledSourceNode_InputCallback, bool, double*);
}

void TestCompilableSourceNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<model::TimeTickType>>(2);
    auto testNode = model.AddNode<nodes::SourceNode<double>>(
        inputNode->output,
        g_inputSize,
        "CompiledSourceNode_InputCallback",
        [] (auto& input)
        {
            input.assign(g_inputSize, 42.0);
            return true;
        });

    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });

    model::MapCompilerParameters settings;
    settings.moduleName = "Test";
    settings.compilerSettings.optimize = true;

    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { { 5, 10 }, { 100, 200 }, { 456, 789} };
    VerifyCompiledOutput(map, compiledMap, signal, "SourceNode");

    // Verify that source callbacks are actually called
    testing::ProcessTest("Testing callback values", testing::IsEqual(g_callbackCount, signal.size()));
}

// C callback (called by emitted code)
extern "C"
{
size_t g_sinkOutputSize = 0;
std::vector<double> outputValues;
void Test_CompiledSinkNode_OutputCallback_Scalar(double output)
{
    Log() << "Sink Output Callback (Scalar) " << output << EOL;
    assert(g_sinkOutputSize == 1);
    outputValues.push_back(output);
}
TESTING_FORCE_DEFINE_SYMBOL(Test_CompiledSinkNode_OutputCallback_Scalar, void, double);

void Test_CompiledSinkNode_OutputCallback_Vector(double* output)
{
    Log() << "Sink Output Callback (Vector) " << *output << EOL;
    assert(g_sinkOutputSize > 1);
    outputValues.assign(output, output + g_sinkOutputSize); // assign reallocates as needed
}
}
TESTING_FORCE_DEFINE_SYMBOL(Test_CompiledSinkNode_OutputCallback_Vector, void, double*);

void TestCompilableSinkNode(size_t inputSize, const std::string& sinkFunctionName)
{
    g_sinkOutputSize = inputSize;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(inputSize);
    auto testNode = model.AddNode<nodes::SinkNode<double>>(inputNode->output, sinkFunctionName);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });

    model::MapCompilerParameters settings;
    settings.moduleName = "Test";
    settings.compilerSettings.optimize = true;

    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { {} };
    for (size_t i = 0; i < inputSize; ++i)
    {
        signal[0].push_back(i * 10);
    }
    outputValues.clear();
    VerifyCompiledOutput(map, compiledMap, signal, "SinkNode");

    // Verify that sink callbacks are actually called
    testing::ProcessTest("Testing callback values", testing::IsEqual(outputValues, signal[0]));
}

void TestCompilableSinkNode()
{
    TestCompilableSinkNode(1, "CompiledSinkNode_OutputCallback_Scalar");
    TestCompilableSinkNode(100, "CompiledSinkNode_OutputCallback_Vector");
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

void TestMultipleOutputNodes()
{
    model::Model model;
    ell::math::TensorShape shape{ 224, 224, 3 };
    auto inputNode = model.AddNode<model::InputNode<double>>(shape);
    auto outputNode = model.AddNode<model::OutputNode<double>>(inputNode->output, shape);

    // this is blocked by IRMapCompiler.cpp line 42 which throws, so uncomment this when we decide to fix that.
    //    auto outputNode2 = model.AddNode<model::OutputNode<double>>(inputNode->output);

    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } }); // , { "output2", outputNode2->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);

    PrintIR(compiledMap);

    std::ostringstream buffer;
    compiledMap.WriteCode(buffer, emitters::ModuleOutputFormat::ir);

    std::string result = buffer.str();

    // some minimal strings for testing, full verbose string comparison might be too fragile to future code gen changes.
    auto inputFuncPos = result.find("define void @ELL_GetInputShape(i32 %index, %TensorShape* %shape");
    auto outputFuncpos = result.find("define void @ELL_GetOutputShape(i32 %index, %TensorShape* %shape");
    auto storePos = result.find("store i32 224, i32* %rows, align 4");
    testing::ProcessTest("Testing GetOutputShape generation",
                         storePos != std::string::npos && inputFuncPos != std::string::npos && outputFuncpos != std::string::npos);
}

void TestMatrixVectorMultiplyNode(int m, int n, bool useBlas)
{
    using ValueType = float;
    std::vector<ValueType> vectorVals(n);
    FillVector(vectorVals);

    model::Model model;
    auto inputMatrixNode = model.AddNode<model::InputNode<ValueType>>(m*n);
    auto inputVectorNode = model.AddNode<nodes::ConstantNode<ValueType>>(vectorVals);

    auto matVecMultNode = model.AddNode<nodes::MatrixVectorMultiplyNode<ValueType>>(inputMatrixNode->output, m, n, n, inputVectorNode->output);

    auto map = model::DynamicMap(model, { { "inputMatrix", inputMatrixNode } }, { { "output", matVecMultNode->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.useBlas = useBlas;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // compare output
    std::vector<ValueType> matrixVals(m * n);
    FillVector(matrixVals);
    std::vector<std::vector<ValueType>> signal = { matrixVals };
    VerifyCompiledOutput(map, compiledMap, signal, "MatrixVectorMultiplyNode");
}

void TestMatrixMatrixMultiplyNode(int m, int n, int k, bool useBlas)
{
    using ValueType = float;
    std::vector<ValueType> matrixBVals(k * n);
    FillVector(matrixBVals);

    model::Model model;
    auto inputMatrixNode = model.AddNode<model::InputNode<ValueType>>(m * k);
    auto matrixBNode = model.AddNode<nodes::ConstantNode<ValueType>>(matrixBVals);

    int lda = k;
    int ldb = n;
    int ldc = n;
    auto matMatMultNode = model.AddNode<nodes::MatrixMatrixMultiplyNode<ValueType>>(inputMatrixNode->output, m, n, k, lda, matrixBNode->output, ldb, ldc);

    auto map = model::DynamicMap(model, { { "inputMatrix", inputMatrixNode } }, { { "output", matMatMultNode->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.useBlas = useBlas;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // compare output
    std::vector<ValueType> matrixAVals(m * k);
    FillVector(matrixAVals);
    std::vector<std::vector<ValueType>> signal = { matrixAVals };
    VerifyCompiledOutput(map, compiledMap, signal, "MatrixMatrixMultiplyNode");
}

// C callback (called by emitted code)
extern "C"
{
void Test_ClockNode_LagNotificationCallback(double lag)
{
    Log() << "ClockNode Lag Notification Callback " << lag << EOL;
}
}
TESTING_FORCE_DEFINE_SYMBOL(Test_ClockNode_LagNotificationCallback, void, double);

void TestCompilableClockNode()
{
    constexpr short lagThreshold = 5;
    constexpr nodes::TimeTickType interval = 50;

    model::Model model;

    auto inputNode = model.AddNode<model::InputNode<nodes::TimeTickType>>(1);
    auto clockNote = model.AddNode<nodes::ClockNode>(inputNode->output, interval, lagThreshold, "ClockNode_LagNotificationCallback");
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", clockNote->output } });

    model::MapCompilerParameters settings;
    settings.moduleName = "Test";
    settings.compilerSettings.optimize = true;

    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare output
    constexpr nodes::TimeTickType thresholdTicks = lagThreshold * interval;
    std::vector<std::vector<nodes::TimeTickType>> signal = {
        { 0 },
        { interval*1 + thresholdTicks/2 }, // within threshold
        { interval*2 }, // on time
        { interval*3 + thresholdTicks }, // late
        { interval*4 + thresholdTicks*20 }, // really late
        { interval*5 } // on time
    };

    VerifyCompiledOutput(map, compiledMap, signal, "ClockNode");
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
    void Copy(model::ModelTransformer& transformer) const override
    {
        auto newInput1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto newInput2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<BinaryFunctionIRNode>(newInput1, newInput2, GetFunctionName(), GetIRCode(), GetExtraArgs());
        transformer.MapNodeOutput(output, newNode->output);
    }

    std::vector<llvm::Value*> GetNodeFunctionStateArguments(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& currentFunction) const override
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
    std::vector<std::vector<ElementType>> signal = { inputWithPadding.ToArray() };
    std::vector<std::vector<ElementType>> expectedOutput = { output.ToArray() };
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
    // Create a simple neural net model with the following layers:
    // input -> bias
    using ElementType = double;
    using InputParameters = typename predictors::neural::InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename predictors::neural::Layer<ElementType>::LayerParameters;
    using VectorType = typename predictors::neural::Layer<ElementType>::VectorType;
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
    // Create a simple neural net model with the following layers:
    // input -> fully-connected -> bias -> activation -> fully-connected -> bias
    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
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
    settings.compilerSettings.parallelize = false;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    PrintIR(compiledMap);
    // compare output
    std::vector<std::vector<double>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_2");
}

void TestNeuralNetworkPredictorNode3()
{
    // Create a simple neural net model with the following layers:
    // input -> convolutional
    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    size_t imageSize = 3;
    size_t numChannels = 2;
    size_t k = 3;
    size_t numFilters = 2;
    size_t padding = 1;
    size_t stride = 1;

    // convolve 3x3 image of all 1s with a 3x3 filter of all 1s, with 0-padding on image
    // result should be:
    //
    //   4 6 4
    //   6 9 6
    //   4 6 4

    InputParameters inputParams = { { imageSize, imageSize, numChannels }, NoPadding(), { imageSize + 2 * padding, imageSize + 2 * padding, numChannels }, { PaddingScheme::zeros, padding }, 1.0 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), { PaddingScheme::zeros, padding }, { imageSize, imageSize, numFilters }, NoPadding() };
    auto convolutionMethod = ConvolutionMethod::columnwise;
    ConvolutionalParameters convolutionalParams{ k, stride, convolutionMethod, numFilters };
    TensorType convWeights(numFilters * k, k, numChannels);
    FillTensor(convWeights);
    // convWeights.Fill(1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights)));

    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

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
    const auto inputSize = imageSize * imageSize * numChannels;
    std::vector<ElementType> input(inputSize, 1.0);
    FillVector(input, 1.0);
    std::vector<std::vector<double>> signal = { input };

    compiledMap.SetInputValue(0, input);
    auto compiledResult = compiledMap.ComputeOutput<ElementType>(0);
    std::cout << "Compiled result size: " << compiledResult.size() << std::endl;
    std::cout << "  " << compiledResult << std::endl;

    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_3");
}

void TestNeuralNetworkPredictorNode4()
{
    // Create a simple neural net model with the following layers:
    // input -> convolutional (no padding)

    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    size_t imageSize = 5;
    size_t numChannels = 1;
    size_t k = 3;
    size_t numFilters = 1;
    size_t padding = 0;
    size_t stride = 1;

    // convolve 5x5 image of all 1s with a 3x3 filter of all 1s, with no padding on image
    // result should be:
    //
    //   9 9 9
    //   9 9 9
    //   9 9 9

    InputParameters inputParams = { { imageSize, imageSize, numChannels }, NoPadding(), { imageSize + 2 * padding, imageSize + 2 * padding, numChannels }, { PaddingScheme::zeros, padding }, 1.0 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), { PaddingScheme::zeros, padding }, { imageSize-2*(k/2), imageSize-2*(k/2), numFilters }, NoPadding() };
    auto convolutionMethod = ConvolutionMethod::columnwise;
    ConvolutionalParameters convolutionalParams{ k, stride, convolutionMethod, numFilters }; //
    TensorType convWeights(numFilters * k, k, numChannels);
    // FillTensor(convWeights);
    convWeights.Fill(1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights)));

    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

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
    const auto inputSize = imageSize * imageSize * numChannels;
    std::vector<ElementType> input(inputSize, 1.0);
    // FillVector(input, 1.0);
    std::vector<std::vector<double>> signal = { input };

    map.SetInputValue(0, input);
    auto computedResult = map.ComputeOutput<ElementType>(0);
    std::cout << "Computed result size: " << computedResult.size() << std::endl;
    std::cout << "  " << computedResult << std::endl;

    compiledMap.SetInputValue(0, input);
    auto compiledResult = compiledMap.ComputeOutput<ElementType>(0);
    std::cout << "Compiled result size: " << compiledResult.size() << std::endl;
    std::cout << "  " << compiledResult << std::endl;

    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_4");
}

void TestNeuralNetworkPredictorNode5()
{
    // Create a simple neural net model with the following layers:
    // input -> convolutional -> max_pool -> convolutional -> mean-pooling -> softmax
    using ElementType = float;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    size_t k = 3;
    size_t w = 8;
    size_t d = 2;
    size_t f1 = 3;
    size_t f2 = 6;

    // Input Layer
    InputParameters inputParams = { { w, w, d }, NoPadding(), { w+2, w+2, d }, ZeroPadding(1), 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    // ConvolutionalLayer
    LayerParameters layerParameters{ inputLayer->GetOutput(), ZeroPadding(1), { w, w, f1 }, NoPadding() };
    auto convolutionMethod = ConvolutionMethod::columnwise;
    ConvolutionalParameters convolutionalParams{ k, 1, convolutionMethod, 1 };
    TensorType convWeights1(f1 * k, k, d);
    FillTensor(convWeights1, -10.0f, 0.0625f);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights1)));

    // Max PoolingLayer
    layerParameters = { layers.back()->GetOutput(), ZeroPadding(1), { w/2 + 2, w/2 + 2, f1 }, ZeroPadding(1) };
    PoolingParameters poolingParameters{ 2, 2 }; // window size, stride
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new PoolingLayer<ElementType, MaxPoolingFunction>(layerParameters, poolingParameters)));

    // ConvolutionalLayer
    layerParameters = { layers.back()->GetOutput(), ZeroPadding(1), { w/2, w/2, f2 }, NoPadding() };
    ConvolutionalParameters convolutionalParams2{ k, 1, convolutionMethod, 1 };
    TensorType convWeights2(f2 * k, k, f1);
    FillTensor(convWeights2, -2.0f, 0.0625f);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams2, convWeights2)));

    // Mean PoolingLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { (w/2)/2, (w/2)/2, f2 }, NoPadding() };
    PoolingParameters poolingParameters2{ 2, 2 };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new PoolingLayer<ElementType, MeanPoolingFunction>(layerParameters, poolingParameters2)));

    // Create the predictor
    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, neuralNetwork);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", predictorNode->output } });

    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    settings.compilerSettings.useBlas = true; // !!! if BLAS is off, this fails
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    // PrintIR(compiledMap);

    // Create an input vector
    std::vector<ElementType> input(w*w*d);
    FillVector(input);

    map.SetInputValue(0, input);
    auto computedResult = map.ComputeOutput<ElementType>(0);
    std::cout << "Computed result size: " << computedResult.size() << std::endl;
    std::cout << "  " << computedResult << std::endl;

    compiledMap.SetInputValue(0, input);
    auto compiledResult = compiledMap.ComputeOutput<ElementType>(0);
    std::cout << "Compiled result size: " << compiledResult.size() << std::endl;
    std::cout << "  " << compiledResult << std::endl;

    // compare output
    std::vector<std::vector<ElementType>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_5");
}

void TestNeuralNetworkPredictorNode6()
{
    // Create a simple neural net model with the following layers:
    // input -> convolutional -> bias -> activation -> batch-norm -> scaling -> max-pooling -> mean-pooling
    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using VectorType = typename Layer<ElementType>::VectorType;

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
    FillTensor(convWeights1, -10.0);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights1)));

    // BiasLayer
    layerParameters = { layers[0]->GetOutput(), NoPadding(), { 3, 3, 8 }, NoPadding() };
    VectorType bias1(layerParameters.outputShape.NumChannels());
    FillVector(bias1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias1)));

    // ActivationLayer
    layerParameters = { layers[1]->GetOutput(), NoPadding(), { 3, 3, 8 }, NoPadding() };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ActivationLayer<ElementType, ReLUActivation>(layerParameters)));

    // BatchNormalizationLayer
    layerParameters = { layers[2]->GetOutput(), NoPadding(), { 3, 3, 8 }, NoPadding() };
    VectorType mean(layerParameters.outputShape.NumChannels());
    VectorType variance(layerParameters.outputShape.NumChannels());
    FillVector(mean);
    FillVector(variance);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BatchNormalizationLayer<ElementType>(layerParameters, mean, variance, 1.0e-6f, EpsilonSummand::SqrtVariance)));

    // ScalingLayer
    layerParameters = { layers[3]->GetOutput(), NoPadding(), { 5, 5, 8 }, { PaddingScheme::zeros, 1 } };
    VectorType scales(layerParameters.outputShape.NumChannels());
    FillVector(scales, -3.0);
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
    FillVector(input);

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
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_6");
}

// tinyYolo prefix test
void TestNeuralNetworkPredictorNode7()
{
    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    // Build a net (a prefix of darknet's tinyYolo)

    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    auto convolutionMethod = ConvolutionMethod::columnwise;
    const Shape inputSize = { 224, 224, 3 };
    const Shape paddedInputSize = { 226, 226, 3 };

    // Input layer
    InputParameters inputParams{ inputSize, NoPadding(), paddedInputSize, ZeroPadding(1), 1.0 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    // layer_0 = ConvolutionalLayer<float>(shape=[224,224,16])
    // ConvolutionalLayer
    LayerParameters layerParameters{ inputLayer->GetOutput(), ZeroPadding(1), { 224, 224, 16 }, NoPadding() };
    ConvolutionalParameters convolutionalParams{ 3, 1, convolutionMethod, 1 };
    TensorType convWeights(16 * 3, 3, 3);
    FillRandomTensor(convWeights);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights)));

    // layer_1 = BatchNormalizationLayer<float>(shape=[224,224,16])
    // BatchNormalizationLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 224, 224, 16 }, NoPadding() };
    VectorType mean(layerParameters.outputShape.NumChannels());
    VectorType variance(layerParameters.outputShape.NumChannels());
    FillRandomVector(mean);
    FillRandomVector(variance, 0.125, 1.0);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BatchNormalizationLayer<ElementType>(layerParameters, mean, variance, 1.0e-6f, EpsilonSummand::SqrtVariance)));

    // layer_2 = ScalingLayer<float>(shape=[224,224,16])
    // ScalingLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 224, 224, 16 }, NoPadding() };
    VectorType scales(layerParameters.outputShape.NumChannels());
    FillRandomVector(scales);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ScalingLayer<ElementType>(layerParameters, scales)));

    // layer_3 = BiasLayer<float>(shape=[224,224,16])
    // BiasLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 224, 224, 16 }, NoPadding() };
    VectorType bias(layerParameters.outputShape.NumChannels());
    FillRandomVector(bias);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, scales)));

    // layer_4 = ActivationLayer<float,LeakyReLUActivation>(shape=[224,224,16])
    // ActivationLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 224, 224, 16 }, NoPadding() };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ActivationLayer<ElementType, LeakyReLUActivation>(layerParameters)));

    // layer_5 = PoolingLayer<float,MaxPoolingFunction>(shape=[114,114,16])
    // Max PoolingLayer
    layerParameters = { layers.back()->GetOutput(), ZeroPadding(1), { 114, 114, 16 }, ZeroPadding(1) };
    PoolingParameters poolingParameters{ 2, 2 };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new PoolingLayer<ElementType, MaxPoolingFunction>(layerParameters, poolingParameters)));

    // layer_6 = ConvolutionalLayer<float>(shape=[112,112,32])
    // ConvolutionalLayer
    layerParameters = { layers.back()->GetOutput(), ZeroPadding(1), { 112, 112, 32 }, NoPadding() };
    convolutionalParams = { 3, 1, convolutionMethod, 1 };
    convWeights = { 32 * 3, 3, 16 };
    FillRandomTensor(convWeights);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights)));

    // layer_7 = BatchNormalizationLayer<float>(shape=[112,112,32])
    // BatchNormalizationLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 112, 112, 32 }, NoPadding() };
    mean = VectorType(layerParameters.outputShape.NumChannels());
    variance = VectorType(layerParameters.outputShape.NumChannels());
    FillRandomVector(mean);
    FillRandomVector(variance, 0.125, 1.0);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BatchNormalizationLayer<ElementType>(layerParameters, mean, variance, 1.0e-4f, EpsilonSummand::SqrtVariance)));

    // layer_8 = ScalingLayer<float>(shape=[112,112,32])
    // ScalingLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 112, 112, 32 }, NoPadding() };
    scales = VectorType(layerParameters.outputShape.NumChannels());
    FillRandomVector(scales);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ScalingLayer<ElementType>(layerParameters, scales)));

    // layer_9 = BiasLayer<float>(shape=[112,112,32])
    // BiasLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 112, 112, 32 }, NoPadding() };
    bias = VectorType(layerParameters.outputShape.NumChannels());
    FillRandomVector(bias);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, scales)));

    // layer_10 = ActivationLayer<float,LeakyReLUActivation>(shape=[112,112,32])
    // ActivationLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 112, 112, 32 }, NoPadding() };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ActivationLayer<ElementType, LeakyReLUActivation>(layerParameters)));

    // layer_11 = PoolingLayer<float,MaxPoolingFunction>(shape=[56,56,32])
    // Max PoolingLayer
    layerParameters = { layers.back()->GetOutput(), ZeroPadding(1), { 58, 58, 32 }, ZeroPadding(1) };
    poolingParameters = { 2, 2 };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new PoolingLayer<ElementType, MaxPoolingFunction>(layerParameters, poolingParameters)));

    // layer_12 = ConvolutionalLayer<float>(shape=[56,56,16])
    // ConvolutionalLayer
    layerParameters = { layers.back()->GetOutput(), ZeroPadding(1), { 56, 56, 16 }, NoPadding() };
    convolutionalParams = { 3, 1, convolutionMethod, 1 };
    convWeights = { 16 * 3, 3, 32 };
    FillRandomTensor(convWeights);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights)));

    // layer_13 = BatchNormalizationLayer<float>(shape=[56,56,16])
    // BatchNormalizationLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 56, 56, 16 }, NoPadding() };
    mean = VectorType(layerParameters.outputShape.NumChannels());
    variance = VectorType(layerParameters.outputShape.NumChannels());
    FillRandomVector(mean);
    FillRandomVector(variance, 0.125, 1.0);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BatchNormalizationLayer<ElementType>(layerParameters, mean, variance, 1.0e-6f, EpsilonSummand::SqrtVariance)));

    // layer_14 = ScalingLayer<float>(shape=[56,56,16])
    // ScalingLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 56, 56, 16 }, NoPadding() };
    scales = VectorType(layerParameters.outputShape.NumChannels());
    FillRandomVector(scales);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ScalingLayer<ElementType>(layerParameters, scales)));

    // layer_15 = BiasLayer<float>(shape=[56,56,16])
    // BiasLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 56, 56, 16 }, NoPadding() };
    bias = VectorType(layerParameters.outputShape.NumChannels());
    FillRandomVector(bias);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, scales)));

    // layer_16 = ActivationLayer<float,LeakyReLUActivation>(shape=[58,58,16])
    // ActivationLayer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), { 58, 58, 16 }, ZeroPadding(1) };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ActivationLayer<ElementType, LeakyReLUActivation>(layerParameters)));

    // layer_17 = ConvolutionalLayer<float>(shape=[56,56,128])
    layerParameters = { layers.back()->GetOutput(), ZeroPadding(1), { 56, 56, 128 }, NoPadding() };
    convolutionalParams = { 3, 1, convolutionMethod, 1 };
    convWeights = { 128 * 3, 3, 16 };
    FillRandomTensor(convWeights);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights)));

    // Create the predictor
    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    std::vector<ElementType> input(inputSize.Size());
    FillRandomVector(input);

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
    double epsilon = 0.0001;
    std::vector<std::vector<double>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, predictorNode->GetRuntimeTypeName() + "_7", epsilon);
}

void TestInputLayerNode(size_t outputPadding)
{
    using ElementType = double;
    using InputParameters = typename predictors::neural::InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename predictors::neural::Layer<ElementType>::LayerParameters;
    using DataVectorType = typename predictors::NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    typename predictors::NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename predictors::NeuralNetworkPredictor<ElementType>::Layers layers;

    // Input layer
    InputParameters inputParams{ { 1, 1, 3 }, predictors::neural::NoPadding(), { 2 * outputPadding + 1, 2 * outputPadding + 1, 3 }, predictors::neural::ZeroPadding(outputPadding), 1.0 };
    inputLayer = std::make_unique<predictors::neural::InputLayer<ElementType>>(inputParams);

    // Pooling layer
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
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;

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
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::ActivationLayerNode<ElementType, ActivationFunction>>(inputNode->output, layer);
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

void TestParametricReLUActivationLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;

    // Build a model
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input(0, 0, 0) = 1.0;
    input(0, 1, 0) = -2.0;
    input(1, 0, 1) = 3.0;
    input(1, 1, 1) = -4.0;
    Shape outputShape = { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };
    LayerParameters layerParameters{ input, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };

    TensorType alphaWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType alpha = alphaWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    alpha(0, 0, 0) = 0.1;
    alpha(0, 1, 0) = 0.2;
    alpha(1, 0, 1) = 0.3;
    alpha(1, 1, 1) = 0.4;

    ParametricReLUActivation<ElementType> prelu(alphaWithPadding);
    ActivationLayer<ElementType, ParametricReLUActivation> layer(layerParameters, prelu);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::ParametricReLUActivationLayerNode<ElementType>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });
    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
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
    Shape outputShape = { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };
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
    Shape outputShape = { 2 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };
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

void TestBinaryConvolutionalLayerNode(size_t imageRows, size_t imageColumns, size_t numChannels, size_t numFilters, size_t inputPaddingSize, size_t outputPaddingSize, ell::predictors::neural::PaddingScheme paddingScheme, bool scaleByFilterMeans)
{
    using ElementType = float;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;

    const size_t k = 3;
    const size_t stride = 1;

    // Verify BinaryConvolutionalLayer with bitwise method
    TensorType inputWithPadding(imageRows + 2 * inputPaddingSize, imageColumns + 2 * inputPaddingSize, numChannels);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, imageRows, imageColumns, numChannels);
    input.Fill(0);
    int inputSize = input.Size();
    FillTensor(input, -2*static_cast<ElementType>(inputSize)/3);

    Shape outputShape = { imageRows + 2 * outputPaddingSize, imageColumns + 2 * outputPaddingSize, numFilters };

    LayerParameters parameters{ inputWithPadding, { paddingScheme, inputPaddingSize }, outputShape, { paddingScheme, outputPaddingSize } };
    BinaryConvolutionalParameters convolutionalParams{ k, stride, BinaryConvolutionMethod::bitwise, scaleByFilterMeans ? BinaryWeightsScale::mean : BinaryWeightsScale::none };
    TensorType weights(convolutionalParams.receptiveField * outputShape.NumChannels(), convolutionalParams.receptiveField, input.NumChannels());
    int weightsSize = weights.Size();
    FillTensor(weights, -static_cast<ElementType>(weightsSize)/2);

    BinaryConvolutionalLayer<ElementType> layer(parameters, convolutionalParams, weights);
    layer.Compute();
    auto output = layer.GetOutput(); unused(output);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::BinaryConvolutionalLayerNode<ElementType>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    // Compile it
    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    settings.compilerSettings.useBlas = true; // !!! if BLAS is off, this fails
    settings.compilerSettings.allowVectorInstructions = false;
    settings.compilerSettings.vectorWidth = 2;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    auto signal = std::vector<std::vector<ElementType>> {inputWithPadding.ToArray()};
    VerifyCompiledOutput<ElementType>(map, compiledMap, {signal}, computeNode->GetRuntimeTypeName());
}

void TestConvolutionalLayerNode(ConvolutionType convolutionType, size_t inputPaddingSize, size_t outputPaddingSize)
{
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;

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
    TensorType weights(convolutionalParams.receptiveField * outputShape.NumChannels(), convolutionalParams.receptiveField, input.NumChannels());
    // clang-format off
    std::vector<ElementType> weightsVector{   // RowMajor then depth order
        1, 3, 2, 3, 1, 1, 2, 3, 1,
        2, 4, 1, 3, 1, 2, 1, 4, 2,
        1, 2, 1, 2, 3, 2, 1, 2, 1,
        0, 3, 2, 3, 1, 2, 1, 0, 2 };
    // clang-format on
    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape.NumChannels(); ++f)
    {
        for (size_t k = 0; k < input.NumChannels(); ++k)
        {
            for (size_t i = 0; i < convolutionalParams.receptiveField; ++i)
            {
                for (size_t j = 0; j < convolutionalParams.receptiveField; ++j)
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
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;

    const size_t numRows = 56;
    const size_t numCols = 56;
    const size_t numChannels = 16;
    const size_t numFilters = 128;

    auto rng = utilities::GetRandomEngine("123");
    auto rand = [&rng]() { return (double)rng() / (double)(rng.max() - rng.min()); };

    assert(inputPaddingSize == 1);
    TensorType inputWithPadding(numRows + 2 * inputPaddingSize, numCols + 2 * inputPaddingSize, numChannels);
    inputWithPadding.Fill(0);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, numRows, numCols, numChannels);
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (size_t colIndex = 0; colIndex < numCols; ++colIndex)
        {
            for (size_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                // input(rowIndex, colIndex, channelIndex) = 1.25 * rowIndex + 0.75 * colIndex + channelIndex;
                input(rowIndex, colIndex, channelIndex) = rand() - 0.5;
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
                // weights(rowIndex, colIndex, channelIndex) = 1.5 * rowIndex + 3.3 * colIndex + 0.15 * channelIndex;
                weights(rowIndex, colIndex, channelIndex) = rand() - 0.5;
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

    // Set up layer
    TensorType inputWithPadding(2 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, 2, 2, 2);
    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;

    auto inputPadding = inputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(inputPaddingSize);
    auto outputPadding = outputPaddingSize == 0 ? predictors::neural::NoPadding() : predictors::neural::ZeroPadding(outputPaddingSize);
    Shape outputShape = { 4 + 2 * outputPaddingSize, 1 + 2 * outputPaddingSize, 1 };
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

template <typename ElementType, template <typename> class PoolingFunction>
void TestPoolingLayerNode(size_t inRows, size_t inCols, size_t numChannels, size_t outRows, size_t outCols, size_t poolingSize, size_t poolingStride, size_t inputPaddingSize, size_t outputPaddingSize, double epsilon)
{
    std::string tname = typeid(ElementType).name();
    Log() << "TestPoolingLayerNode< " << tname << ">([" << inRows << "," << inCols << "],[" << outRows << "," << outCols << "], pool=" << poolingSize << ", stride=" << poolingStride << ", inpad=" << inputPaddingSize << ", outpad=" << outputPaddingSize << ", e=" << epsilon << ")" << EOL;

    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;

    // Build a model
    TensorType inputWithPadding(inRows + 2 * inputPaddingSize, inCols + 2 * inputPaddingSize, numChannels);
    TensorReferenceType input = inputWithPadding.GetSubTensor(inputPaddingSize, inputPaddingSize, 0, inRows, inCols, numChannels);
    FillTensor(input);

    Shape outputShape = { outRows + 2 * outputPaddingSize, outCols + 2 * outputPaddingSize, numChannels };
    LayerParameters layerParameters{ inputWithPadding, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    PoolingParameters poolingParameters{ poolingSize, poolingStride };
    PoolingLayer<ElementType, PoolingFunction> layer(layerParameters, poolingParameters);
    layer.Compute();
    auto output = layer.GetOutput();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::PoolingLayerNode<ElementType, PoolingFunction>>(inputNode->output, layer);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    VerifyLayerMap<ElementType>(map, computeNode, inputWithPadding, output);
}

void TestMaxPoolingLayerNode(size_t inRows, size_t inCols, size_t numChannels, size_t outRows, size_t outCols, size_t poolingSize, size_t poolingStride, size_t inputPaddingSize, size_t outputPaddingSize)
{
    TestPoolingLayerNode<double, predictors::neural::MaxPoolingFunction>(inRows, inCols, numChannels, outRows, outCols, poolingSize, poolingStride, inputPaddingSize, outputPaddingSize, 1e-10);
    TestPoolingLayerNode<float, predictors::neural::MaxPoolingFunction>(inRows, inCols, numChannels, outRows, outCols, poolingSize, poolingStride, inputPaddingSize, outputPaddingSize, 1e-5);
}

void TestMeanPoolingLayerNode(size_t inRows, size_t inCols, size_t numChannels, size_t outRows, size_t outCols, size_t poolingSize, size_t poolingStride, size_t inputPaddingSize, size_t outputPaddingSize)
{
    TestPoolingLayerNode<double, ell::predictors::neural::MeanPoolingFunction>(inRows, inCols, numChannels, outRows, outCols, poolingSize, poolingStride, inputPaddingSize, outputPaddingSize, 1e-10);
    TestPoolingLayerNode<float, ell::predictors::neural::MeanPoolingFunction>(inRows, inCols, numChannels, outRows, outCols, poolingSize, poolingStride, inputPaddingSize, outputPaddingSize, 1e-5);
}

void TestScalingLayerNode(size_t inputPaddingSize, size_t outputPaddingSize)
{
    using ElementType = double;
    using LayerType = predictors::neural::ScalingLayer<ElementType>;
    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using TensorReferenceType = typename LayerType::TensorReferenceType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;

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
    using ElementType = double;
    using LayerType = predictors::neural::SoftmaxLayer<ElementType>;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;

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

void TestFusedLinearLayerNodes(size_t rows, size_t columns, size_t channels)
{
    // Create a simple neural net model with the following layers:
    // input -> scaling -> bias
    using ElementType = double;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using VectorType = typename Layer<ElementType>::VectorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;
    Layer<ElementType>::Shape dataShape = {rows, columns, channels };
    auto dataSize = rows * columns * channels; unused(dataSize);

    // Input layer
    InputParameters inputParams = { dataShape, NoPadding(), dataShape, NoPadding(), 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), NoPadding(), dataShape, NoPadding() };

    // Set up initial bias layer
    layerParameters = { inputLayer->GetOutput(), NoPadding(), dataShape, NoPadding() };
    VectorType bias1(channels);
    FillRandomVector(bias1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias1)));

    // Set up scaling layer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), dataShape, NoPadding() };
    VectorType scale1(channels);
    FillRandomVector(scale1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ScalingLayer<ElementType>(layerParameters, scale1)));

    // Set up bias layer
    layerParameters = { layers.back()->GetOutput(), NoPadding(), dataShape, NoPadding() };
    VectorType bias2(channels);
    FillRandomVector(bias2);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias2)));

    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    std::vector<ElementType> input(rows*columns*channels);
    FillRandomVector(input);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<double>>(inputNode->output, neuralNetwork);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", predictorNode->output } });

    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    settings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare output
    std::vector<std::vector<double>> signal = { input };
    VerifyCompiledOutput(map, compiledMap, signal, "Fused linear layers");
}

//
// Recurrent layer nodes (Recurrent, GRU, LSTM)
//

// clang-format off
const float wData[] = { 0.0381341, 0.55826, -0.467607, 0.264272, -0.733331, 0.464226, 0.496708,
                        0.0581872, -0.514144, 0.702823, -1.50401, 0.373703, 0.885559, -0.27592,
                        -0.116469, 0.320376, -0.534044, 1.92602, -0.567954, -0.0167191, -0.822891 };
// clang-format on

void TestRecurrentNode()
{
    using ElementType = double;
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType biases = VectorType({ -0.0773237, 0.909263, -0.297635 });

    MatrixType weights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            weights(i, j) = wData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output ~ 1,1,0
    input(0, 0, 0) = 5.1;
    input(0, 0, 1) = 3.5;
    input(0, 0, 2) = 1.4;
    input(0, 0, 3) = 0.2;

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };

    RecurrentLayer<ElementType, TanhActivation> recurrent(parameters, weights, biases);
    recurrent.Compute();
    TensorType output = recurrent.GetOutput();

    recurrent.Reset();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(input.Size());
    auto computeNode = model.AddNode<nodes::RecurrentLayerNode<ElementType, TanhActivation>>(inputNode->output, recurrent);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    // Compile model
    model::MapCompilerParameters settings;
    settings.compilerSettings.useBlas = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare computed vs. compiled output
    std::vector<std::vector<ElementType>> signal = { input.ToArray() };
    VerifyCompiledOutput(map, compiledMap, signal, computeNode->GetRuntimeTypeName());
}

// clang-format off
const float uData[] = { -0.306974, -0.314942, -0.307079, -0.0778356, -0.0929513, 0.0426045, -0.0200071,
                        0.508866, 0.525531, 0.345996, -0.633406, -0.519455, 0.617442, -0.0790342,
                        2.13148, 2.61342, -2.99549, -6.15958, 0.224837, 0.0745432, 0.154865 };
const float rData[] = { -0.438305, -0.438798, -0.509791, 0.385411, -0.210201, -0.302488, 0.0717234,
                        0.259852, 0.532692, 0.675258, 0.0314993, -0.609884, -0.419196, 0.407534,
                        0.221932, 0.51503, -0.278936, 0.673416, 0.307534, -0.176314, 0.440408 };
const float hData[] = { 0.0364258, 0.557955, -0.467648, 0.265914, 0.343273, -0.0306102, -0.265686,
                        0.241587, 0.283854, 0.232303, -0.397746, -0.191887, -0.0618932, -0.551409,
                        0.847701, 0.234382, -0.107097, -0.38192, 0.074817, 0.555262, 0.479104 };
// clang-format on

void TestGRUNode()
{
    using ElementType = double;
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType updateBias = VectorType({ 0.0, 0.0, 3.95111 });
    VectorType resetBias = VectorType({ 0.0, 0.0, 0.0 });
    VectorType hiddenBias = VectorType({ -0.0686757, 0.0, 0.281977 });

    MatrixType updateWeights(3, 7);
    MatrixType resetWeights(3, 7);
    MatrixType hiddenWeights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            updateWeights(i, j) = uData[columnIndex];
            resetWeights(i, j) = rData[columnIndex];
            hiddenWeights(i, j) = hData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output ~1,0,0
    input(0, 0, 0) = 5.1;
    input(0, 0, 1) = 3.5;
    input(0, 0, 2) = 1.4;
    input(0, 0, 3) = 0.2;

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };

    GRUParameters<ElementType> gruParams{ updateWeights, resetWeights, hiddenWeights, updateBias, resetBias, hiddenBias };
    GRULayer<ElementType, TanhActivation, SigmoidActivation> gru(parameters, gruParams);
    gru.Compute();
    TensorType output = gru.GetOutput();
    gru.Reset();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(input.Size());
    auto computeNode = model.AddNode<nodes::GRULayerNode<ElementType, TanhActivation, SigmoidActivation>>(inputNode->output, gru);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    // Compile model
    model::MapCompilerParameters settings;
    settings.compilerSettings.useBlas = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare computed vs. compiled output
    std::vector<std::vector<ElementType>> signal = { input.ToArray() };
    VerifyCompiledOutput(map, compiledMap, signal, computeNode->GetRuntimeTypeName());
}

// clang-format off
const float iData[] = { 0.739646, 0.8501, -2.15136, -2.44612, 0.0639512, -0.0492275, 0.167204,
                        -0.49359, 0.253341, -0.239276, 0.114082, -0.360225, 0.434314, -0.28489,
                        -0.573704, -0.0273829, 0.0242156, -0.600619, -0.258574, -0.312928, -0.0446059 };
const float fData[] = { 0.0628231, 0.145727, -0.258802, -0.57547, -0.511279, -0.470488, 0.231888,
                        0.42041, -0.440816, -0.343813, 0.463799, -0.456978, 0.081054, 0.532126,
                        0.51855, -0.123881, 0.509249, 0.324012, 0.318677, -0.411882, 0.082 };
const float cData[] = { 0.187203, 0.863434, 0.490011, -0.216801, -0.290302, 0.338456, -0.216217,
                        -0.000121037, 0.0000392739, 0.00000052499, 0.0000676336, 0.196989, 0.312441, 0.355654,
                        0.468885, -0.236218, 0.415782, 0.302927, -0.0503453, -0.183221, -0.500112 };
const float oData[] = { 0.517059, 0.470772, -0.919974, -0.319515, 0.224966, 0.195129, 0.306053,
                        0.261489, 0.499691, 0.132338, 0.47862, 0.21803, 0.00246173, -0.0274337,
                        -0.385968, 0.120127, -0.360038, -0.21129, 0.0611264, -0.17212, -0.165724 };
// clang-format on
void TestLSTMNode()
{
    using ElementType = double;
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType inputBias = VectorType({ 0.747351, -0.112848, 0.0 });
    VectorType forgetMeBias = VectorType({ 1.0, 1.0, 1.0 });
    VectorType candidateBias = VectorType({ 0.733668, 0.000431956, 0.0 });
    VectorType outputBias = VectorType({ 0.385433, 0.0, 0.0 });

    MatrixType inputWeights(3, 7);
    MatrixType forgetMeWeights(3, 7);
    MatrixType candidateWeights(3, 7);
    MatrixType outputWeights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            inputWeights(i, j) = iData[columnIndex];
            forgetMeWeights(i, j) = fData[columnIndex];
            candidateWeights(i, j) = cData[columnIndex];
            outputWeights(i, j) = oData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output 1,0,0
    input(0, 0, 0) = 5.1;
    input(0, 0, 1) = 3.5;
    input(0, 0, 2) = 1.4;
    input(0, 0, 3) = 0.2;

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };

    LSTMParameters<ElementType> lstmParams{ inputWeights, forgetMeWeights, candidateWeights, outputWeights, inputBias, forgetMeBias, candidateBias, outputBias };

    LSTMLayer<ElementType, TanhActivation, SigmoidActivation> lstm(parameters, lstmParams);
    lstm.Compute();
    TensorType output = lstm.GetOutput();
    lstm.Reset();

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(input.Size());
    auto computeNode = model.AddNode<nodes::LSTMLayerNode<ElementType, TanhActivation, SigmoidActivation>>(inputNode->output, lstm);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    // Compile model
    model::MapCompilerParameters settings;
    settings.compilerSettings.useBlas = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // compare computed vs. compiled output
    std::vector<std::vector<ElementType>> signal = { input.ToArray() };
    VerifyCompiledOutput(map, compiledMap, signal, computeNode->GetRuntimeTypeName());
}
