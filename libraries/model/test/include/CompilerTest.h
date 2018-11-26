////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerTest.h (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model_testing/include/ModelTestUtilities.h>

#include <emitters/include/IREmitter.h>

#include <model/include/CompiledMap.h>
#include <model/include/IRCompiledMap.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Map.h>
#include <model/include/OutputNode.h>

#include <nodes/include/LinearPredictorNode.h>

#include <string>

using namespace ell;

void SetOutputPathBase(std::string path);
std::string OutputPath(std::string relPath);

ell::model::Map MakeSimpleMap();
ell::model::Map MakeForestMap();

void TestBinaryVector(bool expanded, bool runJit = false);
void TestBinaryScalar();
void TestDotProduct();
void TestSimpleSum(bool expanded, bool optimize = false);
void TestSum(bool expanded, bool optimize = false);
void TestAccumulator(bool expanded);
void TestDelay();
void TestSqrt();
void TestBinaryPredicate(bool expanded);
void TestMultiplexer();
void TestSlidingAverage();
void TestDotProductOutput();
template <typename ElementType>
void TestLinearPredictor();
void TestForest();
void TestForestMap();
void TestNodeMetadata();

void TestSimpleMap(bool optimize);
void TestSqEuclideanDistanceMap();
void TestProtoNNPredictorMap();
void TestCombineOutputMap();
void TestMultiOutputMap();
void TestMultiSourceSinkMap();
void TestCompiledMapMove();

#pragma region implementation

template <typename ElementType>
void TestLinearPredictor()
{
    std::vector<std::vector<ElementType>> signal{ { 1.0, 2.0, 1.0, -1.0, 0.5 } };

    const int dim = 5;
    math::ColumnVector<ElementType> weights({ 1, 2, 3, 4, 5 });
    ElementType bias = 1.5f;

    predictors::LinearPredictor<ElementType> predictor(weights, bias);

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(dim);
    auto predictorNode = model.AddNode<nodes::LinearPredictorNode<ElementType>>(inputNode->output, predictor);
    auto outputNode = model.AddNode<model::OutputNode<ElementType>>(predictorNode->output);

    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });

    model::MapCompilerOptions settings;
    settings.mapFunctionName = "TestLinear";
    model::IRMapCompiler compiler(settings);
    auto compiledMap = compiler.Compile(map);

    testing::ProcessTest("Testing IsValid of LinearPredictor map", testing::IsEqual(compiledMap.IsValid(), true));

    // compare output
    VerifyCompiledOutput(map, compiledMap, signal, " map");
}

#pragma endregion implementation
