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
#include "DynamicMap.h"
#include "IRCompiledMap.h"
#include "Model.h"
#include "SteppableMap.h"

// nodes
#include "AccumulatorNode.h"
#include "ConstantNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ForestPredictorNode.h"
#include "LinearPredictorNode.h"
#include "SinkNode.h"
#include "SourceNode.h"
#include "SumNode.h"

// emitters
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IREmitter.h"
#include "IRFunctionEmitter.h"
#include "IRMapCompiler.h"
#include "IRModuleEmitter.h"
#include "IRSteppableMapCompiler.h"
#include "ScalarVariable.h"
#include "VectorVariable.h"

// predictors
#include "LinearPredictor.h"

// clock interface
#include "ClockInterface.h"

// testing
#include "testing.h"

// stl
#include <chrono>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

using namespace ell;

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

model::DynamicMap MakeSimpleMap()
{
    // make a model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto sumNode = model.AddNode<nodes::SumNode<double>>(inputNode->output);

    return model::DynamicMap{ model, { { "input", inputNode } }, { { "output", sumNode->output } } };
}

model::DynamicMap MakeForestMap()
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
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", accumNode2->output } });
    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = optimize;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    testing::ProcessTest("Testing IsValid of original map", testing::IsEqual(compiledMap.IsValid(), true));

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, " map");
}

void TestMultiOutputMap()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto accumNode = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);
    auto outputNode = model.AddNode<model::OutputNode<double>>(model::PortElements<double>{ inputNode->output, accumNode->output });

    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } });
    model::MapCompilerParameters settings;
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

    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } });
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
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", accumNode->output } });
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

    model::MapCompilerParameters settings;
    settings.compilerSettings.unrollLoops = expanded;
    settings.mapFunctionName = modelFunctionName;
    model::IRMapCompiler compiler(settings);

    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", multiplyNode->output } } };
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

    model::MapCompilerParameters settings;
    settings.compilerSettings.optimize = true;
    model::IRMapCompiler compiler(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", addNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestDotProduct(model::MapCompilerParameters& settings)
{
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelMaker mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto dotProduct = mb.DotProduct<double>(c1->output, input1->output);
    auto outputNode = mb.Outputs<double>(dotProduct->output);

    model::IRMapCompiler compiler(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);
}

void TestDotProduct()
{
    model::MapCompilerParameters settings;

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

    model::MapCompilerParameters settings;
    settings.compilerSettings.unrollLoops = expanded;
    settings.compilerSettings.optimize = optimized;
    model::IRMapCompiler compiler(settings);

    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", sumNode->output } } };
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

    model::MapCompilerParameters settings;
    settings.compilerSettings.unrollLoops = expanded;
    settings.compilerSettings.optimize = optimized;
    model::IRMapCompiler compiler(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", sumNode->output } } };
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

    model::MapCompilerParameters settings;
    settings.compilerSettings.unrollLoops = expanded;
    model::IRMapCompiler compiler(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
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
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
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
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
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
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
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
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
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

    model::MapCompilerParameters settings;
    settings.mapFunctionName = "TestSlidingAverage";
    model::IRMapCompiler compiler(settings);

    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
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
    model::MapCompilerParameters settings;
    settings.compilerSettings.inlineOperators = false;
    settings.mapFunctionName = "TestDotProduct";
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelMaker mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto dotProduct = mb.DotProduct<double>(c1->output, input1->output);
    auto outputNode = mb.Outputs<double>(dotProduct->output);

    model::IRMapCompiler compiler(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
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

model::DynamicMap MakeLinearPredictor()
{
    // make a linear predictor
    size_t dim = 3;
    predictors::LinearPredictor predictor(dim);
    predictor.GetBias() = 2.0;
    predictor.GetWeights() = math::ColumnVector<double>{ 3.0, 4.0, 5.0 };

    // make a model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto linearPredictorNode = model.AddNode<nodes::LinearPredictorNode>(inputNode->output, predictor);

    // refine the model
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto newModel = transformer.RefineModel(model, context);

    // check for equality
    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto newOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<double>{ linearPredictorNode->output });
    inputNode->SetInput({ 1.0, 1.0, 1.0 });
    newInputNode->SetInput({ 1.0, 1.0, 1.0 });
    auto modelOutputValue = model.ComputeOutput(linearPredictorNode->output)[0];
    auto newOutputValue = newModel.ComputeOutput(newOutputElements)[0];

    testing::ProcessTest("Testing LinearPredictorNode refine", testing::IsEqual(modelOutputValue, newOutputValue));
    model::DynamicMap map{ newModel, { { "input", newInputNode } }, { { "output", newOutputElements } } };
    return map;
}

void TestLinearPredictor()
{
    auto map = MakeLinearPredictor();

    std::vector<double> data = { 1.0, 1.0, 1.0 };

    model::MapCompilerParameters settings;
    settings.mapFunctionName = "TestLinear";
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    //
    // Generate a Main method to invoke our model
    //
    auto& module = compiledMap.GetModule();
    module.DeclarePrintf();

    auto mainFunction = module.BeginMainFunction();
    llvm::Value* pData = module.ConstantArray("c_data", data);

    llvm::Value* pResult1 = mainFunction.Variable(emitters::VariableType::Double, 1);
    llvm::Value* pResult2 = mainFunction.Variable(emitters::VariableType::Double, 1);
    mainFunction.Call("TestLinear", { mainFunction.PointerOffset(pData, 0), mainFunction.PointerOffset(pResult1, 0), mainFunction.PointerOffset(pResult2, 0) });

    mainFunction.PrintForEach("%f\n", pResult1, 1);
    mainFunction.PrintForEach("%f\n", pResult2, 1);
    mainFunction.Return();
    module.EndFunction();

    PrintIR(module);
    module.WriteToFile(OutputPath("linear.ll"));
}

void TestForest()
{
    auto map = MakeForestMap();

    std::vector<double> data = { 0.2, 0.5, 0.0 };

    model::MapCompilerParameters settings;
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

const std::vector<double> c_steppableMapData{ 1, 3, 5, 7, 9, 11, 13 };
std::vector<double> compiledSteppableMapResults(c_steppableMapData.size());

extern "C"
{
// Callbacks used by compiled map
bool CompiledSteppableMap_DataCallback(double* input)
{
    std::copy(c_steppableMapData.begin(), c_steppableMapData.end(), input);
    return true;
}
const std::string sourceFunctionName("CompiledSteppableMap_DataCallback");

void CompiledSteppableMap_ResultsCallback(double* results)
{
    compiledSteppableMapResults.assign(results, results + c_steppableMapData.size());
}
const std::string sinkFunctionName("CompiledSteppableMap_ResultsCallback");
}

// Ensure that LLVM jit can find these symbols
TESTING_FORCE_DEFINE_SYMBOL(CompiledSteppableMap_DataCallback, bool, double*);
TESTING_FORCE_DEFINE_SYMBOL(CompiledSteppableMap_ResultsCallback, void, double*);

// Callbacks used by dynamic map
bool SteppableMap_DataCallback(std::vector<double>& input)
{
    return CompiledSteppableMap_DataCallback(&input[0]);
}

template <typename ClockType>
void TestSteppableMap(bool runJit, std::function<model::TimeTickType()> getTicksFunction)
{
    const std::string stepFunctionName("TestStep");
    std::vector<double> dynamicMapResults(c_steppableMapData.size());

    // Create the map
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<model::TimeTickType>>(2);
    auto sourceNode = model.AddNode<nodes::SourceNode<double, &SteppableMap_DataCallback>>(inputNode->output, c_steppableMapData.size(), sourceFunctionName);
    auto accumNode = model.AddNode<nodes::AccumulatorNode<double>>(sourceNode->output);
    auto sinkNode = model.AddNode<nodes::SinkNode<double>>(accumNode->output,
                                                           [&dynamicMapResults](const std::vector<double>& results)
                                                           {
                                                               dynamicMapResults.assign(results.begin(), results.end());
                                                           },
                                                           sinkFunctionName);
    auto outputNode = model.AddNode<model::OutputNode<double>>(model::PortElements<double>(sinkNode->output));

    auto duration = std::chrono::milliseconds(20);
    auto map = model::SteppableMap<ClockType>(
        model,
        { { "timeSignal", inputNode } },
        { { "accumulatorOutput", outputNode->output } },
        duration);

    // Compile the map
    model::MapCompilerParameters settings;
    settings.mapFunctionName = stepFunctionName;
    settings.moduleName = "TestStepModule";
    model::IRSteppableMapCompiler<ClockType> compiler(settings);
    auto compiledMap = compiler.Compile(map);

    if (runJit)
    {
        getTicksFunction();

        // Time signal input to the model (currently unused because map internally generates a signal)
        std::vector<std::vector<model::TimeTickType>> timeSignal{ { 0, 0 } };
        compiledSteppableMapResults.clear();

        VerifyCompiledOutput(map, compiledMap, timeSignal, " steppable map");
        testing::ProcessTest("Verifying sink output", testing::IsEqual(dynamicMapResults, compiledSteppableMapResults));
    }
    else
    {
        // Generate a Main method to invoke our model
        auto mainFunction = compiledMap.GetModule().BeginMainDebugFunction();
        emitters::IRFunctionCallArguments args(mainFunction);

        // Time signal input to the model (currently unused because map internally generates a signal)
        std::vector<model::TimeTickType> timeSignal{ 0, 0 };
        args.Append(compiledMap.GetModule().ConstantArray("c_timeSignal", timeSignal));
        auto pResult = args.AppendOutput(emitters::VariableType::Double, c_steppableMapData.size());

        mainFunction.Call(stepFunctionName, args);
        mainFunction.PrintForEach("%f\n", pResult, 1);
        mainFunction.Return();
        compiledMap.GetModule().EndFunction();

        PrintIR(compiledMap);
        compiledMap.GetModule().WriteToFile(OutputPath("step.ll"));
        compiledMap.GetModule().WriteToFile(OutputPath("step.h"));
        compiledMap.GetModule().WriteToFile(OutputPath("step.i"));
    }
}

// Ensure that LLVM jit can find these symbols
TESTING_FORCE_DEFINE_SYMBOL(ELL_GetSteadyClockMilliseconds, double);
TESTING_FORCE_DEFINE_SYMBOL(ELL_GetSystemClockMilliseconds, double);

void TestSteppableMap(bool runJit)
{
    TestSteppableMap<std::chrono::steady_clock>(runJit, []()
    {
        auto ticks = ELL_GetSteadyClockMilliseconds();
        if (IsVerbose())
        {
            std::cout << "ELL_GetSteadyClockMilliseconds() ticks: " << ticks << "\n";
        }
        return ticks;
    });

    // Occassional failures, investigating
    /*
    TestSteppableMap<std::chrono::system_clock>(runJit, []()
    {
        auto ticks = ELL_GetSystemClockMilliseconds();
        if (IsVerbose())
        {
            std::cout << "ELL_GetSystemClockMilliseconds() ticks: " << ticks << "\n";
        }
        return ticks;
    });*/
}
