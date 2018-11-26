////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizerTest.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// #include "ModelTestUtilities.h"

#include <model/optimizer/include/ModelOptimizer.h>

#include <model/include/IRMapCompiler.h>
#include <model/include/InputNode.h>
#include <model/include/Map.h>
#include <model/include/MapCompilerOptions.h>
#include <model/include/PortMemoryLayout.h>

#include <nodes/include/BroadcastFunctionNode.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/MatrixMatrixMultiplyNode.h>
#include <nodes/include/ReorderDataNode.h>

#include <passes/include/FuseLinearOperationsPass.h>
#include <passes/include/StandardPasses.h>

#include <testing/include/testing.h>

#include <algorithm>
#include <iostream>

// set to 1 to print models
#define PRINT_MODELS 0

using namespace ell;

//
// Utility functions
//
void PrintModel(const model::Model& model)
{
    std::cout << "------ Model start ------" << std::endl;
    model.Print(std::cout);
    std::cout << "------ Model end ------" << std::endl;
}

void PrintMap(const model::Map& map)
{
    std::cout << "------ Map start ------" << std::endl;
    map.GetModel().Print(std::cout);
    std::cout << "------ Map end ------" << std::endl;
}

template <typename ValueType>
auto Increment(ValueType start, ValueType inc = static_cast<ValueType>(1))
{
    return [start, inc]() mutable { auto t = start; start += inc; return t; };
}

template <typename ValueType>
model::Map GenerateTestModel(const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, std::vector<std::pair<bool, bool>> functionInfos)
{
    auto numRows = inputLayout.GetActiveSize(0);
    auto numColumns = inputLayout.GetActiveSize(1);
    auto numChannels = inputLayout.GetActiveSize(2);

    // Create a model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ValueType>>(numRows * numColumns * numChannels);
    model::MemoryShape scaleShape({ 1, 1, numChannels });
    model::MemoryShape biasShape({ 1, 1, numChannels });

    model::PortElements<ValueType> prevOutput = inputNode->output;
    int scaleStart = static_cast<ValueType>(1);
    int biasStart = static_cast<ValueType>(2);
    for (auto info : functionInfos)
    {
        nodes::ConstantNode<ValueType>* scaleNode = nullptr;
        nodes::ConstantNode<ValueType>* biasNode = nullptr;
        if (info.first) // has scale
        {
            std::vector<ValueType> scaleValues(numChannels);
            std::generate(scaleValues.begin(), scaleValues.end(), Increment(static_cast<ValueType>(scaleStart)));
            scaleNode = model.AddNode<nodes::ConstantNode<ValueType>>(scaleValues, scaleShape);
        }
        else
        {
            scaleNode = model.AddNode<nodes::ConstantNode<ValueType>>();
        }

        if (info.second) // has bias
        {
            std::vector<ValueType> biasValues(numChannels);
            std::generate(biasValues.begin(), biasValues.end(), Increment(static_cast<ValueType>(biasStart)));
            biasNode = model.AddNode<nodes::ConstantNode<ValueType>>(biasValues, biasShape);
        }
        else
        {
            biasNode = model.AddNode<nodes::ConstantNode<ValueType>>();
        }
        auto functionNode = model.AddNode<nodes::BroadcastLinearFunctionNode<ValueType>>(prevOutput, inputLayout, scaleNode->output, biasNode->output, 2, outputLayout);
        prevOutput = functionNode->output;
        ++scaleStart;
        ++biasStart;
    }

    // Make a map from it
    model::Map map(model, { { "input", inputNode } }, { { "output", prevOutput } });
    return map;
}

//
// Tests
//
void TestFuseLinearOpsPass(std::vector<std::pair<bool, bool>> functionInfos)
{
    using ValueType = float;

    int numRows = 1;
    int numColumns = 1;
    int numChannels = 1;
    model::PortMemoryLayout inputLayout({ numRows, numColumns, numChannels });
    model::PortMemoryLayout outputLayout({ numRows, numColumns, numChannels });

    auto map = GenerateTestModel<ValueType>(inputLayout, outputLayout, functionInfos);
    auto oldSize = map.GetModel().Size();

    // Generate test data
    std::vector<ValueType> testInput(numRows * numColumns * numChannels);
    std::generate(testInput.begin(), testInput.end(), Increment<ValueType>(0.0f));

    // Evaluate it pre-optimization
    map.SetInputValue("input", testInput);
    auto referenceOutput = map.ComputeOutput<ValueType>("output");

    // Initialize pass registry
    passes::AddStandardPassesToRegistry();

    // Optimize it
    model::MapCompilerOptions settings;
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::ModelOptimizer optimizer(settings);
    optimizer.AddPass(std::make_unique<passes::FuseLinearOperationsPass>());
    model::Map optimizedMap(map);
    optimizedMap.Optimize(optimizer);

    auto newSize = optimizedMap.GetModel().Size();
    auto numLinearNodes = functionInfos.size();
    testing::ProcessTest("Testing linear ops count", oldSize == (3 * numLinearNodes) + 1 && newSize == 4);

    // Evaluate model post-optimization
    optimizedMap.SetInputValue("input", testInput);
    auto optimizedOutput = optimizedMap.ComputeOutput<ValueType>("output");
    testing::ProcessTest("Testing result", testing::IsEqual(referenceOutput, optimizedOutput));

    //
    // Now test compiled codepath
    //

    // Compile the model
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    auto newCompiledSize = compiledMap.GetModel().Size();
    testing::ProcessTest("Testing compiled linear ops count", newCompiledSize <= newSize);

    // Evaluate the compiled model
    compiledMap.SetInputValue("input", testInput);
    auto compiledOutput = compiledMap.ComputeOutput<ValueType>("output");
    testing::ProcessTest("Testing compiled result", testing::IsEqual(referenceOutput, compiledOutput));
}

void TestFuseLinearOpsPasses()
{
    std::pair<bool, bool> linear = { true, true };
    std::pair<bool, bool> scale = { true, false };
    std::pair<bool, bool> bias = { false, true };

    TestFuseLinearOpsPass({ linear, linear });
    TestFuseLinearOpsPass({ linear, scale });
    TestFuseLinearOpsPass({ linear, bias });

    TestFuseLinearOpsPass({ scale, linear });
    TestFuseLinearOpsPass({ scale, scale });
    TestFuseLinearOpsPass({ scale, bias });

    TestFuseLinearOpsPass({ bias, linear });
    TestFuseLinearOpsPass({ bias, scale });
    TestFuseLinearOpsPass({ bias, bias });

    TestFuseLinearOpsPass({ bias, scale, linear });
    TestFuseLinearOpsPass({ scale, bias, linear });
    TestFuseLinearOpsPass({ linear, bias, scale });
    TestFuseLinearOpsPass({ linear, scale, bias });
    TestFuseLinearOpsPass({ bias, linear, scale });
    TestFuseLinearOpsPass({ scale, linear, bias });

    TestFuseLinearOpsPass({ linear, scale, scale });
    TestFuseLinearOpsPass({ linear, bias, bias });
}

void TestOptimizeReorderDataNodes1()
{
    using ValueType = float;
    constexpr int m = 4, n = 5, k = 6;
    bool transposeA = false, transposeB = false, transposeC = false;

    auto orderA = transposeA ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto orderB = transposeB ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto orderC = transposeC ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto outputLayout = model::PortMemoryLayout(model::MemoryShape{ m, n }).ReorderedCopy(orderC);

    model::Model model;
    auto inputMatrixNode = model.AddNode<model::InputNode<ValueType>>(model::MemoryShape{ m, k });
    auto reorderedInputMatrixNode = model.AddNode<nodes::ReorderDataNode<ValueType>>(inputMatrixNode->output, orderA);

    std::vector<ValueType> matrixBVals(k * n);
    auto matrixBNode = model.AddNode<nodes::ConstantNode<ValueType>>(matrixBVals, model::MemoryShape{ k, n });
    auto reorderedMatrixBNode = model.AddNode<nodes::ReorderDataNode<ValueType>>(matrixBNode->output, orderB);

    auto matMatMultNode = model.AddNode<nodes::MatrixMatrixMultiplyNode<ValueType>>(reorderedInputMatrixNode->output, reorderedMatrixBNode->output, outputLayout);

    auto map = model::Map(model, { { "inputMatrix", inputMatrixNode } }, { { "output", matMatMultNode->output } });

    //
    // test pass
    //

    auto oldSize = map.GetModel().Size();
#if PRINT_MODELS
    PrintModel(map.GetModel());
#endif

    // Initialize pass registry
    passes::AddStandardPassesToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    auto newSize = compiledMap.GetModel().Size();

#if PRINT_MODELS
    PrintModel(compiledMap.GetModel());
#endif

    testing::ProcessTest("Testing compiled model optimizer", oldSize == 5 && newSize == 3);
}

void TestOptimizeReorderDataNodes2()
{
    using ValueType = float;
    constexpr int m = 4, n = 5, k = 6;
    bool transposeA = false, transposeB = true, transposeC = false;

    auto orderA = transposeA ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto orderB = transposeB ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto orderC = transposeC ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto outputLayout = model::PortMemoryLayout(model::MemoryShape{ m, n }).ReorderedCopy(orderC);

    model::Model model;
    auto inputMatrixNode = model.AddNode<model::InputNode<ValueType>>(model::MemoryShape{ m, k });
    auto reorderedInputMatrixNode = model.AddNode<nodes::ReorderDataNode<ValueType>>(inputMatrixNode->output, orderA);

    std::vector<ValueType> matrixBVals(k * n);
    auto matrixBNode = model.AddNode<nodes::ConstantNode<ValueType>>(matrixBVals, model::MemoryShape{ k, n });
    auto reorderedMatrixBNode = model.AddNode<nodes::ReorderDataNode<ValueType>>(matrixBNode->output, orderB);

    auto matMatMultNode = model.AddNode<nodes::MatrixMatrixMultiplyNode<ValueType>>(reorderedInputMatrixNode->output, reorderedMatrixBNode->output, outputLayout);

    auto map = model::Map(model, { { "inputMatrix", inputMatrixNode } }, { { "output", matMatMultNode->output } });

    //
    // test pass
    //

    auto oldSize = map.GetModel().Size();
#if PRINT_MODELS
    PrintModel(map.GetModel());
#endif

    // Initialize pass registry
    passes::AddStandardPassesToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    auto newSize = compiledMap.GetModel().Size();

#if PRINT_MODELS
    PrintModel(compiledMap.GetModel());
#endif

    testing::ProcessTest("Testing compiled model optimizer", oldSize == 5 && newSize == 4);
}

void TestOptimizeReorderDataNodes3()
{
    using ValueType = float;
    constexpr int m = 4, n = 5, k = 6;
    bool transposeA = true, transposeB = true, transposeC = true;

    auto orderA = transposeA ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto orderB = transposeB ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto orderC = transposeC ? model::DimensionOrder{ 1, 0 } : model::DimensionOrder{ 0, 1 };
    auto outputLayout = model::PortMemoryLayout(model::MemoryShape{ m, n }).ReorderedCopy(orderC);

    model::Model model;
    auto inputMatrixNode = model.AddNode<model::InputNode<ValueType>>(model::MemoryShape{ m, k });
    auto reorderedInputMatrixNode = model.AddNode<nodes::ReorderDataNode<ValueType>>(inputMatrixNode->output, orderA);

    std::vector<ValueType> matrixBVals(k * n);
    auto matrixBNode = model.AddNode<nodes::ConstantNode<ValueType>>(matrixBVals, model::MemoryShape{ k, n });
    auto reorderedMatrixBNode = model.AddNode<nodes::ReorderDataNode<ValueType>>(matrixBNode->output, orderB);

    auto matMatMultNode = model.AddNode<nodes::MatrixMatrixMultiplyNode<ValueType>>(reorderedInputMatrixNode->output, reorderedMatrixBNode->output, outputLayout);

    auto map = model::Map(model, { { "inputMatrix", inputMatrixNode } }, { { "output", matMatMultNode->output } });

    //
    // test pass
    //

    auto oldSize = map.GetModel().Size();
#if PRINT_MODELS
    PrintModel(map.GetModel());
#endif

    // Initialize pass registry
    passes::AddStandardPassesToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    auto newSize = compiledMap.GetModel().Size();

#if PRINT_MODELS
    PrintModel(compiledMap.GetModel());
#endif

    testing::ProcessTest("Testing compiled model optimizer", oldSize == 5 && newSize == 5);
}

void TestOptimizeReorderDataNodes4()
{
    using ValueType = float;
    constexpr int m = 4, n = 5, k = 6;

    auto rowMajor = model::DimensionOrder{ 0, 1 };
    auto colMajor = model::DimensionOrder{ 1, 0 };
    auto outputLayout = model::PortMemoryLayout(model::MemoryShape{ m, n }).ReorderedCopy(colMajor);

    model::Model model;
    auto rowMajorLayout = model::PortMemoryLayout(model::MemoryShape{ m, k }).ReorderedCopy(rowMajor);
    auto colMajorLayout = model::PortMemoryLayout(model::MemoryShape{ m, k }).ReorderedCopy(colMajor);
    auto inputMatrixNode = model.AddNode<model::InputNode<ValueType>>(rowMajorLayout.GetActiveSize());
    auto reorderedInputMatrixNode1 = model.AddNode<nodes::ReorderDataNode<ValueType>>(inputMatrixNode->output, rowMajorLayout, colMajorLayout);
    auto reorderedInputMatrixNode2 = model.AddNode<nodes::ReorderDataNode<ValueType>>(reorderedInputMatrixNode1->output, colMajorLayout, rowMajorLayout);
    auto reorderedInputMatrixNode3 = model.AddNode<nodes::ReorderDataNode<ValueType>>(reorderedInputMatrixNode2->output, rowMajorLayout, rowMajorLayout);

    std::vector<ValueType> matrixBVals(k * n);
    rowMajorLayout = model::PortMemoryLayout(model::MemoryShape{ k, n }).ReorderedCopy(rowMajor);
    colMajorLayout = model::PortMemoryLayout(model::MemoryShape{ k, n }).ReorderedCopy(colMajor);
    auto matrixBNode = model.AddNode<nodes::ConstantNode<ValueType>>(matrixBVals, rowMajorLayout);
    auto reorderedMatrixBNode1 = model.AddNode<nodes::ReorderDataNode<ValueType>>(matrixBNode->output, rowMajorLayout, rowMajorLayout);
    auto reorderedMatrixBNode2 = model.AddNode<nodes::ReorderDataNode<ValueType>>(reorderedMatrixBNode1->output, rowMajorLayout, colMajorLayout);
    auto reorderedMatrixBNode3 = model.AddNode<nodes::ReorderDataNode<ValueType>>(reorderedMatrixBNode2->output, colMajorLayout, colMajorLayout);

    auto matMatMultNode = model.AddNode<nodes::MatrixMatrixMultiplyNode<ValueType>>(reorderedInputMatrixNode3->output, reorderedMatrixBNode3->output, outputLayout);

    auto map = model::Map(model, { { "inputMatrix", inputMatrixNode } }, { { "output", matMatMultNode->output } });

    //
    // test pass
    //

    auto oldSize = map.GetModel().Size();
#if PRINT_MODELS
    PrintModel(map.GetModel());
#endif

    // Initialize pass registry
    passes::AddStandardPassesToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    settings.optimizerSettings.fuseLinearFunctionNodes = true;
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);
    auto newSize = compiledMap.GetModel().Size();

#if PRINT_MODELS
    PrintModel(compiledMap.GetModel());
#endif

    testing::ProcessTest("Testing compiled model optimizer", oldSize == 9 && newSize == 4);
}
