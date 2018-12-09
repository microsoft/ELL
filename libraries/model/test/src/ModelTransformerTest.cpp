////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTransformerTest.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelTransformerTest.h"

#include <model_testing/include/ModelTestUtilities.h>

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>
#include <model/include/PortElements.h>

#include <nodes/include/BinaryOperationNode.h>
#include <nodes/include/UnaryOperationNode.h>

#include <testing/include/testing.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace ell;
using namespace ell::model;
using namespace ell::nodes;
using namespace ell::testing;

// prototypes of sub-tests
void TestCopySubmodel_Full(const Model& model);
void TestCopySubmodel_Prefix(const Model& model);
void TestCopySubmodel_DoublePrefix(const Model& model);

void TestTransformSubmodelOnto_CopyInPlace();
void TestTransformSubmodelOnto_CopyOutOfPlace();
void TestTransformSubmodelOnto_CopyPrefixInPlace();
void TestTransformSubmodelOnto_CopyPrefixOutOfPlace();
void TestTransformSubmodelOnto_ModifyInPlace();
void TestTransformSubmodelOnto_ModifyOutOfPlace();

void TestTransformSubmodelInPlace_Copy();
void TestTransformSubmodelInPlace_CopyPrefix();
void TestTransformSubmodelInPlace_Modify();

namespace
{
const std::vector<const InputPortBase*> noInput = {};
const std::vector<const OutputPortBase*> noOutput = {};
} // namespace

// Transform functions
void CopyNode(const Node& node, ModelTransformer& transformer)
{
    transformer.CopyNode(node);
}

void ModifyFirstDebugNode(const Node& node, ModelTransformer& transformer)
{
    auto debugNode = dynamic_cast<const DebugNode<double, int>*>(&node);
    if (debugNode == nullptr)
    {
        transformer.CopyNode(node);
    }
    else
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(debugNode->input);
        auto newNode = transformer.AddNode<DebugNode<double, int>>(newInputs, 101);
        transformer.MapNodeOutput(debugNode->output, newNode->output);
    }
}

// Tests
void TestCopySubmodel()
{
    // Tests the function:
    //
    // Model ModelTransformer::CopySubmodel(const Submodel& submodel, const TransformContext& context);

    auto model = GetLinearDebugNodeModel(8); // in -> n1 -> n2 -> ... -> n8:out
    FailOnException(TestCopySubmodel_Full, model);
    FailOnException(TestCopySubmodel_Prefix, model);
    FailOnException(TestCopySubmodel_DoublePrefix, model);
}

void TestTransformSubmodelOnto()
{
    // Tests the function:
    //
    // Submodel ModelTransformer::TransformSubmodelOnto(const Submodel& submodel, Model& destModel, const std::vector<const OutputPortBase*>& onto,
    //                                                  const TransformContext& context, const NodeTransformFunction& transformFunction);

    FailOnException(TestTransformSubmodelOnto_CopyInPlace);
    FailOnException(TestTransformSubmodelOnto_CopyOutOfPlace);
    FailOnException(TestTransformSubmodelOnto_CopyPrefixInPlace);
    FailOnException(TestTransformSubmodelOnto_CopyPrefixOutOfPlace);
    FailOnException(TestTransformSubmodelOnto_ModifyInPlace);
    FailOnException(TestTransformSubmodelOnto_ModifyOutOfPlace);
}

void TestTransformSubmodelInPlace()
{
    // Tests the function:
    //
    // Submodel ModelTransformer::TransformSubmodelOnto(const Submodel& submodel, const std::vector<const OutputPortBase*>& onto,
    //                                                  const TransformContext& context, const NodeTransformFunction& transformFunction);

    FailOnException(TestTransformSubmodelInPlace_Copy);
    FailOnException(TestTransformSubmodelInPlace_CopyPrefix);
    FailOnException(TestTransformSubmodelInPlace_Modify);
}

// Individual tests
void TestCopySubmodel_Full(const Model& model)
{
    TransformContext context;
    ModelTransformer transformer;

    Submodel submodel(model, noInput, noOutput);
    auto newModel = transformer.CopySubmodel(submodel, context);
    ProcessTest("TestCopySubmodel_Full", newModel.Size() == model.Size());
}

void TestCopySubmodel_Prefix(const Model& model)
{
    TransformContext context;
    ModelTransformer transformer;

    std::vector<const OutputPortBase*> outputs = { &FindDebugNode(model, 3)->output }; // in -> n1 -> n2 -> n3:out
    Submodel submodel(model, noInput, outputs);
    auto newModel = transformer.CopySubmodel(submodel, context);
    ProcessTest("TestCopySubmodel_Prefix", newModel.Size() == 4);
}

void TestCopySubmodel_DoublePrefix(const Model& model)
{
    TransformContext context;
    ModelTransformer transformer;

    std::vector<const OutputPortBase*> outputs = { &FindDebugNode(model, 3)->output, &FindDebugNode(model, 5)->output }; // in -> n1 -> n2 -> n3 -> n4 -> n5:out
    Submodel submodel(model, noInput, outputs);
    auto newModel = transformer.CopySubmodel(submodel, context);
    ProcessTest("TestCopySubmodel_DoublePrefix", newModel.Size() == 6);
}

void TestTransformSubmodelOnto_CopyInPlace()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    TransformContext context;
    ModelTransformer transformer;

    Submodel submodel(srcModel, noInput, noOutput);
    transformer.TransformSubmodelOnto(submodel, srcModel, noOutput, context, CopyNode);
    ProcessTest("TestTransformSubmodelOnto_CopyInPlace", srcModel.Size() == oldSize);
}

void TestTransformSubmodelOnto_CopyOutOfPlace()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    Model destModel;
    TransformContext context;
    ModelTransformer transformer;

    Submodel submodel(srcModel, noInput, noOutput);
    transformer.TransformSubmodelOnto(submodel, destModel, noOutput, context, CopyNode);
    ProcessTest("TestTransformSubmodelOnto_CopyOutOfPlace", destModel.Size() == oldSize);
}

void TestTransformSubmodelOnto_CopyPrefixInPlace()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    TransformContext context;
    ModelTransformer transformer;

    auto branchPoint = &FindDebugNode(srcModel, 1)->output;
    Submodel submodel(srcModel, noInput, { branchPoint });
    transformer.TransformSubmodelOnto(submodel, srcModel, noOutput, context, CopyNode);
    ProcessTest("TestTransformSubmodelOnto_CopyPrefixInPlace", srcModel.Size() == oldSize);
}

void TestTransformSubmodelOnto_CopyPrefixOutOfPlace()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    Model destModel;
    TransformContext context;
    ModelTransformer transformer;

    auto branchPoint = &FindDebugNode(srcModel, 1)->output;
    Submodel submodel(srcModel, noInput, { branchPoint });
    transformer.TransformSubmodelOnto(submodel, destModel, noOutput, context, CopyNode);
    ProcessTest("TestTransformSubmodelOnto_CopyPrefixOutOfPlace", destModel.Size() == oldSize - 1);
}

void TestTransformSubmodelOnto_ModifyInPlace()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    TransformContext context;
    ModelTransformer transformer;

    Submodel submodel(srcModel, noInput, noOutput);
    transformer.TransformSubmodelOnto(submodel, srcModel, noOutput, context, ModifyFirstDebugNode);
    ProcessTest("TestTransformSubmodelOnto_ModifyInPlace", srcModel.Size() == oldSize + 2);
}

void TestTransformSubmodelOnto_ModifyOutOfPlace()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    Model destModel;
    TransformContext context;
    ModelTransformer transformer;

    Submodel submodel(srcModel, noInput, noOutput);
    transformer.TransformSubmodelOnto(submodel, destModel, noOutput, context, ModifyFirstDebugNode);
    ProcessTest("TestTransformSubmodelOnto_ModifyOutOfPlace", destModel.Size() == oldSize);
}

void TestTransformSubmodelInPlace_Copy()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    TransformContext context;
    ModelTransformer transformer;

    Submodel submodel(srcModel, noInput, noOutput);
    transformer.TransformSubmodelOnto(submodel, noOutput, context, CopyNode);
    ProcessTest("TestTransformSubmodelInPlace_Copy", srcModel.Size() == oldSize);
}

void TestTransformSubmodelInPlace_CopyPrefix()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    TransformContext context;
    ModelTransformer transformer;

    auto branchPoint = &FindDebugNode(srcModel, 1)->output;
    Submodel submodel(srcModel, noInput, { branchPoint });
    transformer.TransformSubmodelOnto(submodel, noOutput, context, CopyNode);
    ProcessTest("TestTransformSubmodelInPlace_CopyPrefix", srcModel.Size() == oldSize);
}

void TestTransformSubmodelInPlace_Modify()
{
    auto srcModel = GetLinearDebugNodeModel(2);
    auto oldSize = srcModel.Size();

    TransformContext context;
    ModelTransformer transformer;

    Submodel submodel(srcModel, noInput, noOutput);
    transformer.TransformSubmodelOnto(submodel, noOutput, context, ModifyFirstDebugNode);
    ProcessTest("TestTransformSubmodelInPlace_Modify", srcModel.Size() == oldSize + 2);
}
