////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerTest.tcc (compile_test)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelTestUtilities.h"

// model
#include "CompiledMap.h"
#include "Map.h"
#include "IRCompiledMap.h"
#include "IREmitter.h"
#include "IRMapCompiler.h"

// nodes
#include "LinearPredictorNode.h"

using namespace ell;

template <typename ElementType>
void TestLinearPredictor()
{
    std::vector<std::vector<ElementType>> signal{{ 1.0, 2.0, 1.0, -1.0, 0.5 }};

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

