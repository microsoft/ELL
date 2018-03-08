////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerTest.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilerTest.h"
#include "ModelMaker.h"
#include "ModelTestUtilities.h"

// model
#include "CompilableNode.h"
#include "Map.h"
#include "IRCompiledMap.h"
#include "Model.h"

// nodes
#include "AccumulatorNode.h"
#include "ClockNode.h"
#include "ConstantNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ForestPredictorNode.h"
#include "L2NormSquaredNode.h"
#include "LinearPredictorNode.h"
#include "MatrixVectorProductNode.h"
#include "ProtoNNPredictorNode.h"
#include "SinkNode.h"
#include "SourceNode.h"
#include "SquaredEuclideanDistanceNode.h"
#include "SumNode.h"

// emitters
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IREmitter.h"
#include "IRFunctionEmitter.h"
#include "IRMapCompiler.h"
#include "IRModuleEmitter.h"
#include "ScalarVariable.h"
#include "VectorVariable.h"

// predictors
#include "LinearPredictor.h"
#include "ProtoNNPredictor.h"

// utilities
#include "Logger.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

using namespace ell;
using namespace ell::logging;

std::string outputBasePath = "";

void SetOutputPathBase(std::string path)
{
    outputBasePath = path;
}

std::string OutputPath(std::string relPath)
{
    return outputBasePath + relPath;
}

//
// Helper functions for constructing example models/maps
//

model::Map MakeSimpleMap()
{
    // make a model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto sumNode = model.AddNode<nodes::SumNode<double>>(inputNode->output);

    return model::Map{ model, { { "input", inputNode } }, { { "output", sumNode->output } } };
}

model::Map MakeForestMap()
{
    // define some abbreviations
    using SplitAction = predictors::SimpleForestPredictor::SplitAction;
    using SplitRule = predictors::SingleElementThresholdPredictor;
    using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;

    // build a forest
    predictors::SimpleForestPredictor forest;
    auto root = forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.3 }, EdgePredictorVector{ -1.0, 1.0 } });
    auto child1 = forest.Split(SplitAction{ forest.GetChildId(root, 0), SplitRule{ 1, 0.6 }, EdgePredictorVector{ -2.0, 2.0 } });
    forest.Split(SplitAction{ forest.GetChildId(child1, 0), SplitRule{ 1, 0.4 }, EdgePredictorVector{ -2.1, 2.1 } });
    forest.Split(SplitAction{ forest.GetChildId(child1, 1), SplitRule{ 1, 0.7 }, EdgePredictorVector{ -2.2, 2.2 } });
    forest.Split(SplitAction{ forest.GetChildId(root, 1), SplitRule{ 2, 0.9 }, EdgePredictorVector{ -4.0, 4.0 } });

    auto root2 = forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.2 }, EdgePredictorVector{ -3.0, 3.0 } });
    forest.Split(SplitAction{ forest.GetChildId(root2, 0), SplitRule{ 1, 0.21 }, EdgePredictorVector{ -3.1, 3.1 } });
    forest.Split(SplitAction{ forest.GetChildId(root2, 1), SplitRule{ 1, 0.22 }, EdgePredictorVector{ -3.2, 3.2 } });

    // build the model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto forestNode = model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);

    return { model, { { "input", inputNode } }, { { "output", forestNode->output } } };
}

//
// Tests
//

void TestSimpleMap(bool optimize)
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    // auto bufferNode = model.AddNode<model::OutputNode<double>>(inputNode->output);
    auto accumNode = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);
    auto accumNode2 = model.AddNode<nodes::AccumulatorNode<double>>(accumNode->output);
    // auto outputNode = model.AddNode<model::OutputNode<double>>(accumNode->output);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", accumNode2->output } });
    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = optimize;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    testing::ProcessTest("Testing IsValid of original map", testing::IsEqual(compiledMap.IsValid(), true));

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, " map");
}

void TestSqEuclideanDistanceMap()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    math::RowMatrix<double> m{
        { 1.2, 1.1, 0.8 },
        { 0.6, 0.9, 1.3 },
        { 0.3, 1.0, 0.4 },
        { -.4, 0.2, -.7 }
    };
    auto sqEuclidDistNode = model.AddNode<nodes::SquaredEuclideanDistanceNode<double, math::MatrixLayout::rowMajor>>(inputNode->output, m);
    auto map = model::Map{ model, { { "input", inputNode } }, { { "output", sqEuclidDistNode->output } } };

    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    testing::ProcessTest("Testing IsValid of original map", testing::IsEqual(compiledMap.IsValid(), true));

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, " map");
}

void TestProtoNNPredictorMap()
{
    // the values of dim, gamma, and matrices come from the result of running protoNNTrainer with the following command line
    // protoNNTrainer -v --inputDataFilename Train-28x28_sparse.txt -dd 784 -sw 0.29 -sb 0.8 -sz 0.8 -pd 15 -l 10 -mp 5 --outputModelFilename mnist-94.model --evaluationFrequency 1 -plf L4 -ds 0.003921568627451

    size_t dim = 784, projectedDim = 15, numPrototypes = 50, numLabels = 10;
    double gamma = 0.0733256;
    predictors::ProtoNNPredictor protonnPredictor(dim, projectedDim, numPrototypes, numLabels, gamma);

    // projectedDim * dim
    auto W = protonnPredictor.GetProjectionMatrix() =
    {
        #include "TestProtoNNPredictorMap_Projection.inc"
    };

    // projectedDim * numPrototypes
    auto B = protonnPredictor.GetPrototypes() =
    {
        #include "TestProtoNNPredictorMap_Prototypes.inc"
    };

    // numLabels * numPrototypes
    auto Z = protonnPredictor.GetLabelEmbeddings() =
    {
        #include "TestProtoNNPredictorMap_LabelEmbeddings.inc"
    };

    // MNIST training data features
    std::vector<std::vector<double>> features =
    {
        #include "TestProtoNNPredictorMap_features.inc"
    };

    std::vector<std::vector<int>> labels{ { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 } };

    testing::IsEqual(protonnPredictor.GetProjectedDimension(), projectedDim);
    testing::IsEqual(protonnPredictor.GetNumPrototypes(), numPrototypes);
    testing::IsEqual(protonnPredictor.GetNumLabels(), numLabels);

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(dim);
    auto protonnPredictorNode = model.AddNode<nodes::ProtoNNPredictorNode>(inputNode->output, protonnPredictor);
    auto outputNode = model.AddNode<model::OutputNode<double>>(protonnPredictorNode->output);
    auto map = model::Map{ model, { { "input", inputNode } }, { { "output", outputNode->output } } };

    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = false;
    settings.compilerSettings.includeDiagnosticInfo = true;
    settings.compilerSettings.inlineOperators = false;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    testing::ProcessTest("Testing IsValid of original map", testing::IsEqual(compiledMap.IsValid(), true));

    for (unsigned i = 0; i < features.size(); ++i)
    {
        auto& input = features[i];
        std::transform(input.begin(), input.end(), input.begin(), [](double d) { return d / 255; });

        const auto& label = labels[i];

        IsEqual(input.size(), dim);

        inputNode->SetInput(input);
        auto computeOutput = model.ComputeOutput(outputNode->output);
        testing::ProcessTest("one hot indices are incorrect for computed and actual label",
                             IsEqual(std::max_element(label.begin(), label.end()) - label.begin(),
                                     std::max_element(computeOutput.begin(), computeOutput.end()) - computeOutput.begin()));

        map.SetInputValue(0, input);
        auto refinedOutput = map.ComputeOutput<double>(0);
        testing::ProcessTest("computed and refined output vectors don't match", IsEqual(computeOutput, refinedOutput, 1e-5));

        compiledMap.SetInputValue(0, input);
        auto compiledOutput = compiledMap.ComputeOutput<double>(0);
        testing::ProcessTest("refined and compiled output vectors don't match", IsEqual(refinedOutput, compiledOutput, 1e-5));
    }
}

void TestMultiOutputMap()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto accumNode = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);
    auto outputNode = model.AddNode<model::OutputNode<double>>(model::PortElements<double>{ inputNode->output, accumNode->output });

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerOptions settings;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    testing::ProcessTest("Testing IsValid of original map", testing::IsEqual(compiledMap.IsValid(), true));

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, " map");
}

// Map with 2 scalar values concatenated
void TestMultiOutputMap2()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto sumNode = model.AddNode<nodes::SumNode<double>>(inputNode->output);
    auto dotNode = model.AddNode<nodes::DotProductNode<double>>(inputNode->output, inputNode->output);
    auto outputNode = model.AddNode<model::OutputNode<double>>(model::PortElements<double>{ sumNode->output, dotNode->output });

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "AccumulatorNodeAsFunction");
}

void TestCompiledMapMove()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto accumNode = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", accumNode->output } });
    model::IRMapCompiler compiler1;
    auto compiledMap1 = compiler1.Compile(map);

    PrintIR(compiledMap1);
    testing::ProcessTest("Testing IsValid of original map", testing::IsEqual(compiledMap1.IsValid(), true));

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap1, signal, " moved compiled map");

    auto compiledMap2 = std::move(compiledMap1);
    testing::ProcessTest("Testing IsValid of moved-from map", testing::IsEqual(compiledMap1.IsValid(), false));
    testing::ProcessTest("Testing IsValid of moved-to map", testing::IsEqual(compiledMap2.IsValid(), true));

    // compare output
    VerifyCompiledOutput(map, compiledMap2, signal, " moved compiled map");
}

typedef void (*MapPredictFunction)(double*, double*);

void TestBinaryVector(bool expanded, bool runJit)
{
    std::vector<double> data = { 5, 10, 15, 20 };
    std::vector<double> data2 = { 4, 4, 4, 4 };
    const int c_InputSize = data.size();
    const std::string modelFunctionName = "TestBinaryVector";
    ModelMaker mb;

    auto input1 = mb.Inputs<double>(c_InputSize);
    auto c1 = mb.Constant<double>(data);
    auto c2 = mb.Constant<double>(data2);

    auto bop = mb.Add(c1->output, input1->output);
    auto multiplyNode = mb.Multiply(bop->output, c2->output);

    model::MapCompilerOptions settings;
    settings.compilerSettings.unrollLoops = expanded;
    settings.mapFunctionName = modelFunctionName;
    model::IRMapCompiler compiler(settings);

    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", multiplyNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);

    std::vector<double> testInput = { 1, 1, 1, 1 };
    std::vector<double> testOutput(testInput.size());
    PrintIR(compiledMap);
    PrintDiagnostics(compiledMap.GetModule().GetDiagnosticHandler());
    if (runJit)
    {
        auto& jitter = compiledMap.GetJitter();
        auto predict = reinterpret_cast<MapPredictFunction>(jitter.ResolveFunctionAddress(modelFunctionName));
        predict(&testInput[0], testOutput.data());
    }
    else
    {
        auto mainFunction = compiledMap.GetModule().BeginMainDebugFunction();
        emitters::IRFunctionCallArguments args(mainFunction);
        args.Append(compiledMap.GetModule().ConstantArray("c_data", testInput));
        auto* pResult = args.AppendOutput(emitters::VariableType::Double, testInput.size());
        mainFunction.Call(modelFunctionName, args);
        mainFunction.PrintForEach("%f,", pResult, testInput.size());
        mainFunction.Return();
        compiledMap.GetModule().EndFunction();
        compiledMap.GetModule().WriteToFile(OutputPath(expanded ? "BinaryVector_E.asm" : "BinaryVector.asm"));
    }
}

void TestBinaryScalar()
{
    std::vector<double> data = { 5 };

    ModelMaker mb;
    auto input1 = mb.Inputs<double>(1);
    auto c1 = mb.Constant<double>(data);

    auto addNode = mb.Add(c1->output, input1->output);

    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", addNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestDotProduct(model::MapCompilerOptions& settings)
{
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelMaker mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto dotProduct = mb.DotProduct(c1->output, input1->output);
    auto outputNode = mb.Outputs<double>(dotProduct->output);

    model::IRMapCompiler compiler(settings);
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestDotProduct()
{
    model::MapCompilerOptions settings;

    settings.compilerSettings.unrollLoops = false;
    settings.compilerSettings.inlineOperators = true;
    TestDotProduct(settings);

    settings.compilerSettings.unrollLoops = true;
    settings.compilerSettings.inlineOperators = true;
    TestDotProduct(settings);

    settings.compilerSettings.unrollLoops = false;
    settings.compilerSettings.inlineOperators = false;
    TestDotProduct(settings);
}

void TestSimpleSum(bool expanded, bool optimized)
{
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelMaker mb;
    auto input1 = mb.Inputs<double>(4);
    auto sumNode = mb.Sum<double>(input1->output);

    model::MapCompilerOptions settings;
    settings.compilerSettings.unrollLoops = expanded;
    settings.compilerSettings.optimize = optimized;
    model::IRMapCompiler compiler(settings);

    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", sumNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
    PrintDiagnostics(compiledMap.GetModule().GetDiagnosticHandler());
}

void TestSum(bool expanded, bool optimized)
{
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelMaker mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto product = mb.Multiply<double>(c1->output, input1->output);
    auto sumNode = mb.Sum<double>(product->output);

    model::MapCompilerOptions settings;
    settings.compilerSettings.unrollLoops = expanded;
    settings.compilerSettings.optimize = optimized;
    model::IRMapCompiler compiler(settings);
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", sumNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // Print out any diagnostic messages
    PrintDiagnostics(compiledMap.GetModule().GetDiagnosticHandler());
}

void TestAccumulator(bool expanded)
{
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelMaker mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto product = mb.Multiply<double>(c1->output, input1->output);
    auto accumulate = mb.Accumulate<double>(product->output);
    auto outputNode = mb.Outputs<double>(accumulate->output);

    model::MapCompilerOptions settings;
    settings.compilerSettings.unrollLoops = expanded;
    model::IRMapCompiler compiler(settings);
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestDelay()
{
    ModelMaker mb;
    auto input1 = mb.Inputs<double>(4);
    auto delay = mb.Delay<double>(input1->output, 3);
    auto outputNode = mb.Outputs<double>(delay->output);

    model::IRMapCompiler compiler;
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestSqrt()
{
    ModelMaker mb;
    auto input1 = mb.Inputs<double>(1);
    auto sqrt = mb.Sqrt<double>(input1->output);
    auto outputNode = mb.Outputs<double>(sqrt->output);

    model::IRMapCompiler compiler;
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestBinaryPredicate(bool expanded)
{
    std::vector<double> data = { 5 };

    ModelMaker mb;
    auto input1 = mb.Inputs<double>(data.size());
    auto c1 = mb.Constant<double>(data);
    auto eq = mb.Equals(input1->output, c1->output);
    auto outputNode = mb.Outputs<bool>(eq->output);

    model::IRMapCompiler compiler;
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestMultiplexer()
{
    ModelMaker mb;

    std::vector<double> data = { 5, 10 };
    auto c1 = mb.Constant<bool>(true);
    auto input1 = mb.Inputs<double>(data.size());
    auto selector = mb.Select<double, bool>(input1->output, c1->output);
    auto outputNode = mb.Outputs<double>(selector->output);

    model::IRMapCompiler compiler;
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestSlidingAverage()
{
    ModelMaker mb;
    auto dim = mb.Constant<double>(4);
    auto input1 = mb.Inputs<double>(4);
    auto delay = mb.Delay<double>(input1->output, 2);
    auto sum = mb.Sum<double>(delay->output);
    auto avg = mb.Divide<double>(sum->output, dim->output);
    auto outputNode = mb.Outputs<double>(avg->output);

    model::MapCompilerOptions settings;
    settings.mapFunctionName = "TestSlidingAverage";
    model::IRMapCompiler compiler(settings);

    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);

    auto& module = compiledMap.GetModule();
    module.DeclarePrintf();
    auto mainFunction = module.BeginMainFunction();
    std::vector<double> data = { 5, 10, 15, 20 };
    llvm::Value* pData = module.ConstantArray("c_data", data);
    llvm::Value* pResult = mainFunction.Variable(emitters::VariableType::Double, 1);
    mainFunction.Call("TestSlidingAverage", { mainFunction.PointerOffset(pData, 0), mainFunction.PointerOffset(pResult, 0) });
    mainFunction.PrintForEach("%f\n", pResult, 1);
    mainFunction.Call("TestSlidingAverage", { mainFunction.PointerOffset(pData, 0), mainFunction.PointerOffset(pResult, 0) });
    mainFunction.PrintForEach("%f\n", pResult, 1);
    mainFunction.Call("TestSlidingAverage", { mainFunction.PointerOffset(pData, 0), mainFunction.PointerOffset(pResult, 0) });
    mainFunction.PrintForEach("%f\n", pResult, 1);
    mainFunction.Return();
    module.EndFunction();

    PrintIR(module);
    module.WriteToFile(OutputPath("avg.ll"));
}

void TestDotProductOutput()
{
    model::MapCompilerOptions settings;
    settings.compilerSettings.inlineOperators = false;
    settings.mapFunctionName = "TestDotProduct";
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelMaker mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto dotProduct = mb.DotProduct(c1->output, input1->output);
    auto outputNode = mb.Outputs<double>(dotProduct->output);

    model::IRMapCompiler compiler(settings);
    model::Map map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);

    auto mainFunction = compiledMap.GetModule().BeginMainDebugFunction();
    emitters::IRFunctionCallArguments args(mainFunction);
    args.Append(compiledMap.GetModule().ConstantArray("c_data", data));
    auto pResult = args.AppendOutput(emitters::VariableType::Double, 1);
    mainFunction.Call("TestDotProduct", args);
    mainFunction.PrintForEach("%f\n", pResult, 1);
    mainFunction.Return();
    compiledMap.GetModule().EndFunction();

    PrintIR(compiledMap);
    compiledMap.GetModule().WriteToFile(OutputPath("dot.ll"));
}

void TestForest()
{
    auto map = MakeForestMap();

    std::vector<double> data = { 0.2, 0.5, 0.0 };

    model::MapCompilerOptions settings;
    settings.compilerSettings.optimize = true;
    settings.compilerSettings.includeDiagnosticInfo = false;
    settings.mapFunctionName = "TestForest";
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    auto& module = compiledMap.GetModule();
    module.DeclarePrintf();

    auto mainFunction = module.BeginMainFunction();
    llvm::Value* pData = module.ConstantArray("c_data", data);
    llvm::Value* pResult = nullptr;
    auto args = module.GetFunction("TestForest")->args();
    emitters::IRValueList callArgs;
    callArgs.push_back(mainFunction.PointerOffset(pData, 0));
    size_t i = 0;
    for (auto& arg : args)
    {
        (void)arg; // stifle compiler warning
        if (i > 0)
        {
            llvm::Value* pArg = nullptr;
            if (pResult == nullptr)
            {
                pArg = mainFunction.Variable(emitters::VariableType::Double, 1);
                pResult = pArg;
            }
            else
            {
                pArg = mainFunction.Variable(emitters::VariableType::Int32, 1);
            }
            callArgs.push_back(mainFunction.PointerOffset(pArg, 0));
        }
        ++i;
    }

    //mainFunction.Call("TestForest", { mainFunction.PointerOffset(pData, 0), mainFunction.PointerOffset(pResult, 0) });
    mainFunction.Print("Calling TestForest\n");
    mainFunction.Call("TestForest", callArgs);
    mainFunction.Print("Done Calling TestForest\n");

    mainFunction.PrintForEach("%f\n", pResult, 1);
    mainFunction.Return();
    mainFunction.Verify();

    PrintIR(compiledMap);
    compiledMap.GetModule().WriteToFile(OutputPath("forest_map.ll"));
}

extern "C"
{
// Callbacks used by compiled map
bool TestMulti_DataCallback1(double* input)
{
    Log() << "Data callback 1" << EOL;
    const std::vector<double> input1{ 1, 3, 5, 7, 9, 11, 13 };
    std::copy(input1.begin(), input1.end(), input);
    return true;
}
bool TestMulti_DataCallback2(double* input)
{
    Log() << "Data callback 2" << EOL;
    const std::vector<double> input2{ 42 };
    std::copy(input2.begin(), input2.end(), input);
    return true;
}

void TestMulti_ResultsCallback_Scalar(double result)
{
    Log() << "Results callback (scalar): " << result << EOL;
}

void TestMulti_ResultsCallback_Vector(double* result)
{
    Log() << "Results callback (vector): " << result[0] << EOL;
}

void TestMulti_LagNotificationCallback(double lag)
{
    Log() << "Lag callback:" << lag << EOL;
}
}

// Ensure that LLVM jit can find these symbols
TESTING_FORCE_DEFINE_SYMBOL(TestMulti_DataCallback1, bool, double*);
TESTING_FORCE_DEFINE_SYMBOL(TestMulti_DataCallback2, bool, double*);
TESTING_FORCE_DEFINE_SYMBOL(TestMulti_ResultsCallback_Scalar, void, double);
TESTING_FORCE_DEFINE_SYMBOL(TestMulti_ResultsCallback_Vector, void, double*);
TESTING_FORCE_DEFINE_SYMBOL(TestMulti_LagNotificationCallback, void, double);

void TestMultiSourceSinkMap(bool expanded, bool optimized)
{
    // Create the map
    constexpr nodes::TimeTickType lagThreshold = 200;
    constexpr nodes::TimeTickType interval = 40;

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<nodes::TimeTickType>>(1 /*currentTime*/);
    auto clockNode = model.AddNode<nodes::ClockNode>(inputNode->output, interval, lagThreshold,
        "LagNotificationCallback");
    auto sourceNode1 = model.AddNode<nodes::SourceNode<double>>(clockNode->output, 7,
        "DataCallback1", [] (auto& v) { return TestMulti_DataCallback1(&v[0]); });
    auto sourceNode2 = model.AddNode<nodes::SourceNode<double>>(clockNode->output, 1,
        "DataCallback2", [] (auto& v) { return TestMulti_DataCallback2(&v[0]); });
    auto sumNode = model.AddNode<nodes::SumNode<double>>(sourceNode1->output);
    auto minusNode = model.AddNode<nodes::BinaryOperationNode<double>>(sumNode->output,
        sourceNode2->output, emitters::BinaryOperationType::subtract);
    auto conditionNode = model.AddNode<nodes::ConstantNode<bool>>(true);
    auto sinkNode1 = model.AddNode<nodes::SinkNode<double>>(sumNode->output, conditionNode->output,
        "ResultsCallback_Scalar");
    auto sinkNode2 = model.AddNode<nodes::SinkNode<double>>(model::PortElements<double>{ minusNode->output, sumNode->output },
        conditionNode->output,
        "ResultsCallback_Vector");

    // compiled maps require a single output, so we concatenate the ports for the sink nodes
    auto outputNode = model.AddNode<model::OutputNode<double>>(model::PortElements<double>{ sinkNode1->output, sinkNode2->output });
    auto map = model::Map(model, { { "time", inputNode } }, { { "output", outputNode->output } });

    // Compile the map
    model::MapCompilerOptions settings;
    settings.moduleName = "TestMulti";
    settings.compilerSettings.optimize = optimized;
    settings.compilerSettings.unrollLoops = expanded;

    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    // Compare output
    std::vector<std::vector<nodes::TimeTickType>> signal =
    {
        { 0 },
        { interval*1 + lagThreshold/2 }, // within threshold
        { interval*2 }, // on time
        { interval*3 + lagThreshold }, // late
        { interval*4 + lagThreshold*20 }, // really late
        { interval*5 } // on time
    };

    VerifyCompiledOutput(map, compiledMap, signal, " multi-sink and source map");
}

void TestMultiSourceSinkMap()
{
    TestMultiSourceSinkMap(true, true);
    TestMultiSourceSinkMap(true, false);
    TestMultiSourceSinkMap(false, true);
    TestMultiSourceSinkMap(false, false);
}
