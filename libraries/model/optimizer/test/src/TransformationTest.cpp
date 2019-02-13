////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformationTest.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TransformationTest.h"
#include "ExampleTransformations.h"
#include "FuseLinearOperationsTransformation.h"
#include "OptimizerTestUtil.h"

#include <model/include/Transformation.h>

#include <testing/include/testing.h>

#include <utilities/include/JsonArchiver.h>

#include <iostream>

using namespace ell;
using namespace ell::emitters;
using namespace ell::model;
using namespace ell::model::optimizer;
using namespace ell::testing;
using namespace ell::utilities;

//
// Transformation class tests
//

void TestTransformations()
{
    TestTrivialTransformation();
    TestAddMetadataToOutputTransformation();
    TestCombineNodesTransformation();
    TestFuseLinearOperationsTransformation();
}

void TestTrivialTransformation()
{
    Submodel m = GetSimpleSubmodel();
    ModelTransformer transformer;
    TransformContext context;
    auto oldModelSize = m.GetModel().Size();
    TrivialTransformation t;
    auto newSubmodel = t.Transform(m, transformer, context);

    // TODO: verify that submodels are in fact identical
    ProcessTest("TrivialTransformResultSameSize", SubmodelsAreSameSize(m, newSubmodel));
    ProcessTest("TrivialTransformModelSameSize", oldModelSize == newSubmodel.GetModel().Size());
}

void TestAddMetadataToOutputTransformation()
{
    Submodel m = GetSimpleSubmodel();
    ModelTransformer transformer;
    TransformContext context;
    auto oldModelSize = m.GetModel().Size();
    AddMetadataToOutputTransformation t;
    auto newSubmodel = t.Transform(m, transformer, context);

    ProcessTest("AddMetadataToOutputTransformationResultSameSize", SubmodelsAreSameSize(m, newSubmodel));
    ProcessTest("AddMetadataToOutputTransformationModelNotSameSize", oldModelSize != newSubmodel.GetModel().Size());

    bool ok1 = AllNodesOf(m, [](const Node& node) {
        return !node.GetMetadata().HasEntry("a");
    });
    ProcessTest("AddMetadataToOutputTransformationMetadataCheck1", ok1);

    bool ok2 = AllNodesOf(newSubmodel, [](const Node& node) {
        return !IsOutputNode(node) || node.GetMetadata().HasEntry("a");
    });
    ProcessTest("AddMetadataToOutputTransformationMetadataCheck2", ok2);
}

void TestCombineNodesTransformation()
{
    auto testData = GetCombineNodesTestData();
    const Submodel& m = testData.submodel;
    ModelTransformer transformer;
    TransformContext context;
    auto oldModelSize = (int)m.GetModel().Size();
    CombineNodesTransformation t;

    auto newSubmodel = t.Transform(m, transformer, context);

    ProcessTest("TestCombineNodesTransformationResultNotSameSize", !SubmodelsAreSameSize(m, newSubmodel));
    ProcessTest("TestCombineNodesTransformationModelNotSameSize", oldModelSize != (int)newSubmodel.GetModel().Size());

    ProcessTest("TestCombineNodesTransformationModelSize", oldModelSize == testData.initialModelSize);
    ProcessTest("TestCombineNodesTransformationInitialSubmodelSize", m.Size() == testData.initialSubmodelSize);
    ProcessTest("TestCombineNodesTransformationTransformedSubmodelSize", newSubmodel.Size() == testData.transformedSubmodelSize);
}

void TestFuseLinearOperationsTransformation()
{
#if 0
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
#endif
}