////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformationTest.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TransformationTest.h"
#include "ExampleTransformations.h"
#include "OptimizerTestUtil.h"

#include <model/include/Transformation.h>

#include <testing/include/testing.h>

#include <utilities/include/JsonArchiver.h>

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
