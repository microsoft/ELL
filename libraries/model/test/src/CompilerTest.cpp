////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerTest.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilerTest.h"
#include "CompilerTestUtilities.h"
#include "ModelBuilder.h"

// model
#include "CompilableNode.h"
#include "DynamicMap.h"
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRCompiledMap.h"
#include "IREmitter.h"
#include "IRFunctionEmitter.h"
#include "IRMapCompiler.h"
#include "IRModuleEmitter.h"
#include "Model.h"
#include "ScalarVariable.h"
#include "VectorVariable.h"

// testing
#include "testing.h"

// predictors
#include "LinearPredictor.h"

// nodes
#include "AccumulatorNode.h"
#include "ConstantNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ForestPredictorNode.h"
#include "LinearPredictorNode.h"
#include "SumNode.h"

// stl
#include <iostream>
#include <ostream>
#include <string>

namespace ell
{

std::string g_outputBasePath = "";
void SetOutputPathBase(std::string path)
{
    g_outputBasePath = std::move(path);
}

std::string OutputPath(const char* pRelPath)
{
    return g_outputBasePath + pRelPath;
}

std::string OutputPath(const std::string& relPath)
{
    return g_outputBasePath + relPath;
}

void Print(double* pData, int length)
{
    for (int i = 0; i < length; ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        std::cout << pData[i];
    }
    std::cout << std::endl;
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

model::DynamicMap MakeUnrefinedForestMap()
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
    auto simpleForestNode = model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);

    return { model, { { "input", inputNode } }, { { "output", simpleForestNode->output } } };
}

model::Model MakeForestModel()
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
    model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);

    // refine
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);
    return refinedModel;
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

    // refine
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, context);

    return { model, { { "input", inputNode } }, { { "output", forestNode->output } } };
}

//
// Tests
//

void TestCompiledMapMove()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto accumNode = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", accumNode->output } });
    auto compiledMap1 = model::IRCompiledMap(map);
    testing::ProcessTest("Testing IsValid of original map", testing::IsEqual(compiledMap1.IsValid(), true));

    auto compiledMap2 = std::move(compiledMap1);
    testing::ProcessTest("Testing IsValid of moved-from map", testing::IsEqual(compiledMap1.IsValid(), false));
    testing::ProcessTest("Testing IsValid of moved-to map", testing::IsEqual(compiledMap2.IsValid(), true));

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap2, signal, " moved compiled map");
}

typedef void (*FnInputOutput)(double*, double*);
void TestBinaryVector(bool expanded, bool runJit)
{
    std::vector<double> data = { 5, 10, 15, 20 };
    std::vector<double> data2 = { 4, 4, 4, 4 };
    const int c_InputSize = 4;
    const std::string c_ModelFnName = "TestBinaryVector";
    ModelBuilder mb;

    auto input1 = mb.Inputs<double>(c_InputSize);
    auto c1 = mb.Constant<double>(data);
    auto c2 = mb.Constant<double>(data2);

    auto bop = mb.Add(c1->output, input1->output);
    auto multiplyNode = mb.Multiply(bop->output, c2->output);

    model::IRMapCompiler compiler;
    emitters::CompilerParameters settings;
    settings.unrollLoops = expanded;
    compiler.SetCompilerParameters(settings);

    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", multiplyNode->output } } };
    model::IRCompiledMap compiledMap(map);
    compiler.CompileMap(compiledMap, c_ModelFnName);

    std::vector<double> testInput = { 1, 1, 1, 1 };
    double testOutput[c_InputSize];
    if (runJit)
    {
        auto jitter = compiler.Jit();
        FnInputOutput fn = (FnInputOutput)jitter->ResolveFunctionAddress(c_ModelFnName);
        fn(&testInput[0], testOutput);
        std::cout << c_ModelFnName << std::endl;
        Print(testOutput, testInput.size());
    }
    else
    {
        compiler.DebugDump();

        auto fnMain = compiler.AddMainDebug();

        emitters::IRFunctionCallArguments args(fnMain);
        args.Append(compiler.Constant("c_data", testInput));
        auto* pResult = args.AppendOutput(emitters::VariableType::Double, testInput.size());
        fnMain.Call(c_ModelFnName, args);
        fnMain.PrintForEach("%f,", pResult, testInput.size());
        fnMain.Return();
        fnMain.Complete();

        compiler.WriteToFile(OutputPath(expanded ? "BinaryVector_E.asm" : "BinaryVector.asm"));
    }
}

void TestBinaryScalar()
{
    std::vector<double> data = { 5 };
    std::vector<double> data2 = { 4 };

    ModelBuilder mb;

    auto input1 = mb.Inputs<double>(1);
    auto c1 = mb.Constant<double>(data);

    auto addNode = mb.Add(c1->output, input1->output);

    model::IRMapCompiler compiler("ELL");
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", addNode->output } } };
    model::IRCompiledMap compiledMap{ map };
    compiler.CompileMap(compiledMap, "TestBinaryScalar");
    compiler.DebugDump();
}

void TestDotProduct(emitters::CompilerParameters& settings)
{
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelBuilder mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto dotProduct = mb.DotProduct<double>(c1->output, input1->output);
    auto outputNode = mb.Outputs<double>(dotProduct->output);

    model::IRMapCompiler compiler("ELL");
    compiler.SetCompilerParameters(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap{ map };
    compiler.CompileMap(compiledMap, "TestDotProduct");
    compiler.DebugDump();
}

void TestDotProduct()
{
    emitters::CompilerParameters settings;

    settings.unrollLoops = false;
    settings.inlineOperators = true;
    TestDotProduct(settings);

    settings.unrollLoops = true;
    settings.inlineOperators = true;
    TestDotProduct(settings);

    settings.unrollLoops = false;
    settings.inlineOperators = false;
    TestDotProduct(settings);
}

void TestSum(bool expanded)
{
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelBuilder mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto product = mb.Multiply<double>(c1->output, input1->output);
    auto sumNode = mb.Sum<double>(product->output);

    model::IRMapCompiler compiler("ELL");
    emitters::CompilerParameters settings;
    settings.unrollLoops = expanded;
    compiler.SetCompilerParameters(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", sumNode->output } } };
    model::IRCompiledMap compiledMap{ map };
    compiler.CompileMap(compiledMap, "TestSum");
    compiler.DebugDump();
}

void TestAccumulator(bool expanded)
{
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelBuilder mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto product = mb.Multiply<double>(c1->output, input1->output);
    auto accumulate = mb.Accumulate<double>(product->output);
    auto outputNode = mb.Outputs<double>(accumulate->output);

    model::IRMapCompiler compiler("ELL");
    emitters::CompilerParameters settings;
    settings.unrollLoops = expanded;
    compiler.SetCompilerParameters(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap{ map };

    compiler.CompileMap(compiledMap, "TestAccumulator");
    compiler.DebugDump();
}

void TestDelay()
{
    ModelBuilder mb;
    auto input1 = mb.Inputs<double>(4);
    auto delay = mb.Delay<double>(input1->output, 3);
    auto outputNode = mb.Outputs<double>(delay->output);

    model::IRMapCompiler compiler("ELL");
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap{ map };

    compiler.CompileMap(compiledMap, "TestDelay");
    compiler.DebugDump();
}

void TestSqrt()
{
    ModelBuilder mb;
    auto input1 = mb.Inputs<double>(1);
    auto sqrt = mb.Sqrt<double>(input1->output);
    auto outputNode = mb.Outputs<double>(sqrt->output);

    model::IRMapCompiler compiler("ELL");
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap{ map };

    compiler.CompileMap(compiledMap, "TestSqrt");
    compiler.DebugDump();
}

void TestBinaryPredicate(bool expanded)
{
    std::vector<double> data = { 5 };

    ModelBuilder mb;
    auto input1 = mb.Inputs<double>(data.size());
    auto c1 = mb.Constant<double>(data);
    auto eq = mb.Equals(input1->output, c1->output);
    auto outputNode = mb.Outputs<bool>(eq->output);

    model::IRMapCompiler compiler("ELL");
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap{ map };

    compiler.CompileMap(compiledMap, "TestPredicate");
    compiler.DebugDump();
}

void TestMultiplexer()
{
    ModelBuilder mb;

    std::vector<double> data = { 5, 10 };
    auto c1 = mb.Constant<bool>(true);
    auto input1 = mb.Inputs<double>(data.size());
    auto selector = mb.Select<double, bool>(input1->output, c1->output);
    //selector->output.SetOutput(0);
    // auto outputNode = mb.Outputs<double>(*mb.GetOutputPort<double>(selector, 0));
    auto outputNode = mb.Outputs<double>(selector->output);

    model::IRMapCompiler compiler("ELL");
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap{ map };

    compiler.CompileMap(compiledMap, "TestElementSelector");
    compiler.DebugDump();
}

void TestSlidingAverage()
{
    ModelBuilder mb;
    auto dim = mb.Constant<double>(4);
    auto input1 = mb.Inputs<double>(4);
    auto delay = mb.Delay<double>(input1->output, 2);
    auto sum = mb.Sum<double>(delay->output);
    auto avg = mb.Divide<double>(sum->output, dim->output);
    auto outputNode = mb.Outputs<double>(avg->output);

    model::IRMapCompiler compiler("ELL");
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap{ map };
    compiler.CompileMap(compiledMap, "TestSlidingAverage");

    auto& module = compiler;
    module.DeclarePrintf();
    auto fnMain = module.AddMain();
    std::vector<double> data = { 5, 10, 15, 20 };
    llvm::Value* pData = module.Constant("c_data", data);
    llvm::Value* pResult = fnMain.Variable(emitters::VariableType::Double, 1);
    fnMain.Call("TestSlidingAverage", { fnMain.PointerOffset(pData, 0), fnMain.PointerOffset(pResult, 0) });
    fnMain.PrintForEach("%f\n", pResult, 1);
    fnMain.Call("TestSlidingAverage", { fnMain.PointerOffset(pData, 0), fnMain.PointerOffset(pResult, 0) });
    fnMain.PrintForEach("%f\n", pResult, 1);
    fnMain.Call("TestSlidingAverage", { fnMain.PointerOffset(pData, 0), fnMain.PointerOffset(pResult, 0) });
    fnMain.PrintForEach("%f\n", pResult, 1);
    fnMain.Return();
    fnMain.Complete();

    compiler.DebugDump();
    compiler.WriteToFile(OutputPath("avg.asm"));
}

void TestDotProductOutput()
{
    emitters::CompilerParameters settings;
    settings.inlineOperators = false;
    std::vector<double> data = { 5, 10, 15, 20 };

    ModelBuilder mb;
    auto c1 = mb.Constant<double>(data);
    auto input1 = mb.Inputs<double>(4);
    auto dotProduct = mb.DotProduct<double>(c1->output, input1->output);
    auto outputNode = mb.Outputs<double>(dotProduct->output);

    model::IRMapCompiler compiler("ELL");
    compiler.SetCompilerParameters(settings);
    model::DynamicMap map{ mb.Model, { { "input", input1 } }, { { "output", outputNode->output } } };
    model::IRCompiledMap compiledMap{ map };
    compiler.CompileMap(compiledMap, "TestDotProduct");

    auto fnMain = compiler.AddMainDebug();
    emitters::IRFunctionCallArguments args(fnMain);
    args.Append(compiler.Constant("c_data", data));
    auto pResult = args.AppendOutput(emitters::VariableType::Double, 1);
    fnMain.Call("TestDotProduct", args);
    fnMain.PrintForEach("%f\n", pResult, 1);
    fnMain.Return();
    fnMain.Complete();

    compiler.DebugDump();
    compiler.WriteToFile(OutputPath("dot.asm"));
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

    model::IRMapCompiler compiler;
    compiler.CompileMap(map, "TestLinear");

    //
    // Generate a Main method to invoke our model
    //
    auto& module = compiler;
    module.DeclarePrintf();

    auto fnMain = module.AddMain();
    llvm::Value* pData = module.Constant("c_data", data);

    llvm::Value* pResult1 = fnMain.Variable(emitters::VariableType::Double, 1);
    llvm::Value* pResult2 = fnMain.Variable(emitters::VariableType::Double, 1);
    fnMain.Call("TestLinear", { fnMain.PointerOffset(pData, 0), fnMain.PointerOffset(pResult1, 0), fnMain.PointerOffset(pResult2, 0) });

    fnMain.PrintForEach("%f\n", pResult1, 1);
    fnMain.PrintForEach("%f\n", pResult2, 1);
    fnMain.Return();
    fnMain.Complete();

    compiler.DebugDump();
    compiler.WriteToFile(OutputPath("linear.asm"));
}

void TestForest()
{
    auto map = MakeForestMap();

    std::vector<double> data = { 0.2, 0.5, 0.0 };

    model::IRMapCompiler compiler;
    emitters::CompilerParameters settings;
    settings.optimize = true;
    settings.includeDiagnosticInfo = false;
    compiler.SetCompilerParameters(settings);
    compiler.CompileMap(map, "TestForest");

    auto& module = compiler;
    module.DeclarePrintf();

    auto fnMain = module.AddMain();
    llvm::Value* pData = module.Constant("c_data", data);
    llvm::Value* pResult = nullptr;
    auto args = module.GetFunction("TestForest")->args();
    emitters::IRValueList callArgs;
    callArgs.Append(fnMain.PointerOffset(pData, 0));
    size_t i = 0;
    for (auto& arg : args)
    {
        if (i > 0)
        {
            llvm::Value* pArg = nullptr;
            if (pResult == nullptr)
            {
                pArg = fnMain.Variable(emitters::VariableType::Double, 1);
                pResult = pArg;
            }
            else
            {
                pArg = fnMain.Variable(emitters::VariableType::Int32, 1);
            }
            callArgs.Append(fnMain.PointerOffset(pArg, 0));
        }
        ++i;
    }
    //fnMain.Call("TestForest", { fnMain.PointerOffset(pData, 0), fnMain.PointerOffset(pResult, 0) });
    fnMain.Print("Calling TestForest\n");
    fnMain.Call("TestForest", callArgs);
    fnMain.Print("Done Calling TestForest\n");

    fnMain.PrintForEach("%f\n", pResult, 1);
    fnMain.Return();
    fnMain.Verify();

    compiler.DebugDump();
    compiler.WriteToFile(OutputPath("forest.asm"));
}

void TestForestMap()
{
    std::cout << "Testing forest map" << std::endl;
    auto map = MakeUnrefinedForestMap();
    model::TransformContext context;
    map.Refine(context);

    std::vector<double> data = { 0.2, 0.5, 0.0 };

    model::IRMapCompiler compiler;
    emitters::CompilerParameters settings;
    settings.optimize = true;
    settings.includeDiagnosticInfo = false;
    compiler.SetCompilerParameters(settings);
    compiler.CompileMap(map, "TestForest");

    auto& module = compiler;
    module.DeclarePrintf();

    auto fnMain = module.AddMain();
    llvm::Value* pData = module.Constant("c_data", data);
    llvm::Value* pResult = nullptr;
    auto args = module.GetFunction("TestForest")->args();
    emitters::IRValueList callArgs;
    callArgs.Append(fnMain.PointerOffset(pData, 0));
    size_t i = 0;
    for (auto& arg : args)
    {
        if (i > 0)
        {
            llvm::Value* pArg = nullptr;
            if (pResult == nullptr)
            {
                pArg = fnMain.Variable(emitters::VariableType::Double, 1);
                pResult = pArg;
            }
            else
            {
                pArg = fnMain.Variable(emitters::VariableType::Int32, 1);
            }
            callArgs.Append(fnMain.PointerOffset(pArg, 0));
        }
        ++i;
    }

    //fnMain.Call("TestForest", { fnMain.PointerOffset(pData, 0), fnMain.PointerOffset(pResult, 0) });
    fnMain.Print("Calling TestForest\n");
    fnMain.Call("TestForest", callArgs);
    fnMain.Print("Done Calling TestForest\n");

    fnMain.PrintForEach("%f\n", pResult, 1);
    fnMain.Return();
    fnMain.Verify();

    compiler.DebugDump();
    compiler.WriteToFile(OutputPath("forest_map.asm"));
}
}
