////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizerTest.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// #include "ModelTestUtilities.h"

#include <model/include/IRMapCompiler.h>
#include <model/include/InputNode.h>
#include <model/include/Map.h>
#include <model/include/MapCompilerOptions.h>
#include <model/include/OptimizeModelTransformation.h>
#include <model/include/PortMemoryLayout.h>

#include <nodes/include/BroadcastFunctionNode.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/ConvolutionalLayerNode.h>
#include <nodes/include/MatrixMatrixMultiplyNode.h>
#include <nodes/include/ReorderDataNode.h>

#include <passes/include/StandardTransformations.h>

#include <predictors/neural/include/ConvolutionalLayer.h>

#include <predictors/neural/include/ConvolutionalLayer.h>

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

bool HasNodeWithTypeName(const model::Model& model, std::string typeName)
{
    auto iter = model.GetNodeIterator();
    while (iter.IsValid())
    {
        auto node = iter.Get();
        if (node->GetRuntimeTypeName() == typeName)
        {
            return true;
        }
        iter.Next();
    }
    return false;
}

template <typename ValueType>
auto Increment(ValueType start, ValueType inc = static_cast<ValueType>(1))
{
    return [start, inc]() mutable { auto t = start; start += inc; return t; };
}

template <typename ValueType>
model::Map GenerateLinearOpsTestModel(const model::PortMemoryLayout& inputLayout, const model::PortMemoryLayout& outputLayout, std::vector<std::pair<bool, bool>> functionInfos)
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

    auto map = GenerateLinearOpsTestModel<ValueType>(inputLayout, outputLayout, functionInfos);
    auto oldSize = map.GetModel().Size();

    // Generate test data
    std::vector<ValueType> testInput(numRows * numColumns * numChannels);
    std::generate(testInput.begin(), testInput.end(), Increment<ValueType>(0.0f));

    // Evaluate it pre-optimization
    map.SetInputValue("input", testInput);
    auto referenceOutput = map.ComputeOutput<ValueType>("output");

    // Initialize transformation registry
    passes::AddStandardTransformationsToRegistry();

    // Optimize it
    model::MapCompilerOptions settings;
    model::ModelOptimizerOptions optimizerOptions;
    optimizerOptions["fuseLinearFunctionNodes"] = true;
    model::IRMapCompiler compiler(settings, optimizerOptions);

    model::Map optimizedMap(map);
    model::TransformContext context(&compiler);
    model::OptimizeModelTransformation optimizer;
    optimizedMap.Transform(optimizer, context);
    optimizedMap.Prune();

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
    auto compiledMap = compiler.Compile(map);
    auto newCompiledSize = compiledMap.GetModel().Size();
    testing::ProcessTest("Testing compiled linear ops count", newCompiledSize <= newSize);

    // Evaluate the compiled model
    compiledMap.SetInputValue("input", testInput);
    auto compiledOutput = compiledMap.ComputeOutput<ValueType>("output");
    testing::ProcessTest("Testing compiled result", testing::IsEqual(referenceOutput, compiledOutput));
}

void TestFuseLinearOpsPass()
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
    passes::AddStandardTransformationsToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    model::ModelOptimizerOptions optimizerOptions;
    optimizerOptions["fuseLinearFunctionNodes"] = true;
    model::IRMapCompiler compiler(settings, optimizerOptions);
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
    passes::AddStandardTransformationsToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    model::ModelOptimizerOptions optimizerOptions;
    optimizerOptions["fuseLinearFunctionNodes"] = true;
    model::IRMapCompiler compiler(settings, optimizerOptions);
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
    passes::AddStandardTransformationsToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    model::ModelOptimizerOptions optimizerOptions;
    optimizerOptions["fuseLinearFunctionNodes"] = true;
    model::IRMapCompiler compiler(settings, optimizerOptions);
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
    passes::AddStandardTransformationsToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    model::ModelOptimizerOptions optimizerOptions;
    optimizerOptions["fuseLinearFunctionNodes"] = true;
    model::IRMapCompiler compiler(settings, optimizerOptions);
    auto compiledMap = compiler.Compile(map);
    auto newSize = compiledMap.GetModel().Size();

#if PRINT_MODELS
    PrintModel(compiledMap.GetModel());
#endif

    testing::ProcessTest("Testing compiled model optimizer", oldSize == 9 && newSize == 4);
}

void TestSetConvolutionMethodPass(model::PreferredConvolutionMethod convolutionMethod, std::string expectedNodeTypeName)
{
    using namespace predictors::neural;

    using ElementType = float;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using TensorReferenceType = typename Layer<ElementType>::TensorReferenceType;
    using Shape = typename Layer<ElementType>::Shape;

    const size_t inputPaddingSize = 1;
    const int outputPaddingSize = 0;
    TensorType inputWithPadding(1 + 2 * inputPaddingSize, 2 + 2 * inputPaddingSize, 2);
    TensorReferenceType input = inputWithPadding.GetSubTensor({ inputPaddingSize, inputPaddingSize, 0 }, { 1, 2, 2 });
    inputWithPadding.Fill(0);
    input(0, 0, 0) = 2;
    input(0, 1, 0) = 1;
    input(0, 0, 1) = 3;
    input(0, 1, 1) = 2;
    // Input channel 0: [2, 3], input channel 1: [1, 2]

    Shape outputShape = { 1 + 2 * outputPaddingSize, 2 + 2 * outputPaddingSize, 2 };

    LayerParameters parameters{ inputWithPadding, ZeroPadding(inputPaddingSize), outputShape, ZeroPadding(outputPaddingSize) };
    ConvolutionalParameters convolutionalParams{ 3, 1, ConvolutionMethod::automatic, 2 };

    TensorType weights(convolutionalParams.receptiveField * outputShape.NumChannels(), convolutionalParams.receptiveField, input.NumChannels());
    ConvolutionalLayer<ElementType> layer(parameters, convolutionalParams, weights);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputWithPadding.Size());
    auto computeNode = model.AddNode<nodes::ConvolutionalLayerNode<ElementType>>(inputNode->output, layer);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", computeNode->output } });

    //
    // test pass
    //

#if PRINT_MODELS
    PrintModel(map.GetModel());
#endif

    // Initialize pass registry
    passes::AddStandardTransformationsToRegistry();

    // Compile it
    model::MapCompilerOptions settings;
    model::ModelOptimizerOptions optimizerOptions;
    optimizerOptions["preferredConvolutionMethod"] = convolutionMethod;
    model::IRMapCompiler compiler(settings, optimizerOptions);
    auto compiledMap = compiler.Compile(map);

#if PRINT_MODELS
    PrintModel(compiledMap.GetModel());
#endif

    testing::ProcessTest("Testing SetConvolutionMethodPass for " + expectedNodeTypeName, HasNodeWithTypeName(compiledMap.GetModel(), expectedNodeTypeName));
}

void TestSetConvolutionMethodPass()
{
    TestSetConvolutionMethodPass(model::PreferredConvolutionMethod::diagonal, "DiagonalConvolutionComputeNode<float>");
    TestSetConvolutionMethodPass(model::PreferredConvolutionMethod::simple, "SimpleConvolutionComputeNode<float>");
    TestSetConvolutionMethodPass(model::PreferredConvolutionMethod::winograd, "WinogradConvolutionComputeNode<float>");
    TestSetConvolutionMethodPass(model::PreferredConvolutionMethod::unrolled, "ReceptiveFieldMatrixNode<float>");
}
