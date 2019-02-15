////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizerOptions_test.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelOptimizerOptions_test.h"

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/ModelOptimizerOptions.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputNode.h>
#include <model/include/SetCompilerOptionsTransformation.h>
#include <model/include/Submodel.h>
#include <model/include/TransformContext.h>

#include <testing/include/testing.h>

#include <utilities/include/JsonArchiver.h>

#include <iostream>

using namespace ell;
using namespace ell::model;
using namespace ell::testing;

namespace
{
bool OptionsEqual(const ModelOptimizerOptions& a, const ModelOptimizerOptions& b)
{
    std::vector<std::string> interestingOptions = { "fuseLinearFunctionNodes", "optimizeReorderDataNodes", "preferredConvolutionMethod" };
    for (auto s : interestingOptions)
    {
        if (a.HasEntry(s) != b.HasEntry(s))
        {
            return false;
        }
        else if (a.HasEntry(s) && a.GetEntry(s).IsType<bool>() && a.GetEntry<bool>(s) != b.GetEntry<bool>(s))
        {
            return false;
        }
        else if (a.HasEntry(s) && a.GetEntry(s).IsType<model::PreferredConvolutionMethod>() &&
                 a.GetEntry<model::PreferredConvolutionMethod>(s) != b.GetEntry<model::PreferredConvolutionMethod>(s))
        {
            return false;
        }
    }
    return true;
}

template <typename ModelOrNode>
bool HasSameOptionsInMetadata(const ModelOrNode& n, const ModelOptimizerOptions& options)
{
    if (!n.GetMetadata().HasEntry("compileOptions"))
    {
        return false;
    }

    utilities::Variant nodeOptionsVariant = n.GetMetadata().GetEntry("compileOptions");
    if (!nodeOptionsVariant.IsType<utilities::PropertyBag>())
    {
        return false;
    }

    auto newOptions = nodeOptionsVariant.GetValue<utilities::PropertyBag>();
    return OptionsEqual(options, ModelOptimizerOptions{ newOptions });
}
} // namespace

void TestModelOptimizerOptions()
{
    TestArchiveModelOptimizerOptions();
    TestModelOptimizerOptionsMetadata();
}

void TestArchiveModelOptimizerOptions()
{
    // Create optimizer options
    ModelOptimizerOptions options;
    options["fuseLinearFunctionNodes"] = false;
    options["optimizeReorderDataNodes"] = true;
    options["preferredConvolutionMethod"] = PreferredConvolutionMethod::diagonal;

    // archive
    utilities::PropertyBag properties;
    AppendOptionsToMetadata(options, properties);

    // unarchive
    ProcessTest("Testing ModelOptimizerOptions storage in a PropertyBag", OptionsEqual(options, ModelOptimizerOptions{ properties }));
}

void TestModelOptimizerOptionsMetadata()
{
    Model model;
    auto n1 = model.AddNode<InputNode<float>>(1);
    auto n2 = model.AddNode<OutputNode<float>>(n1->output);
    auto n3 = model.AddNode<OutputNode<float>>(n2->output);

    ModelOptimizerOptions modelOptions;
    modelOptions["fuseLinearFunctionNodes"] = false;
    modelOptions["optimizeReorderDataNodes"] = true;
    modelOptions["preferredConvolutionMethod"] = PreferredConvolutionMethod::diagonal;

    ModelOptimizerOptions node1Options;
    node1Options["fuseLinearFunctionNodes"] = true;
    node1Options["optimizeReorderDataNodes"] = true;
    node1Options["preferredConvolutionMethod"] = PreferredConvolutionMethod::diagonal;

    ModelOptimizerOptions node3Options;
    node3Options["fuseLinearFunctionNodes"] = false;
    node3Options["optimizeReorderDataNodes"] = false;
    node3Options["preferredConvolutionMethod"] = PreferredConvolutionMethod::simple;

    utilities::PropertyBag nodeProperties;
    nodeProperties[n1->GetId().ToString()] = node1Options.AsPropertyBag();
    nodeProperties[n3->GetId().ToString()] = node3Options.AsPropertyBag();

    utilities::PropertyBag properties;
    properties["model"] = modelOptions.AsPropertyBag();
    properties["nodes"] = nodeProperties;

    SetCompilerOptionsTransformation transformation(properties);
    ModelTransformer transformer;
    TransformContext context;
    Submodel submodel{ model };
    auto newSubmodel = transformation.Transform(submodel, transformer, context);

    ProcessTest("Checking old model options metadata", IsFalse(HasSameOptionsInMetadata(model, modelOptions)));
    ProcessTest("Checking old node 1 options metadata", IsFalse(HasSameOptionsInMetadata(*n1, node1Options)));
    ProcessTest("Checking old node 1 options metadata", IsFalse(HasSameOptionsInMetadata(*n3, node3Options)));

    // Check new model
    auto newNode1 = transformer.GetCorrespondingOutputs(n1->output).GetNode();
    auto newNode3 = transformer.GetCorrespondingOutputs(n3->output).GetNode();
    ProcessTest("Checking new model options metadata", IsTrue(HasSameOptionsInMetadata(newSubmodel.GetModel(), modelOptions)));
    ProcessTest("Checking new node 1 options metadata", IsTrue(HasSameOptionsInMetadata(*newNode1, node1Options)));
    ProcessTest("Checking new node 1 options metadata", IsTrue(HasSameOptionsInMetadata(*newNode3, node3Options)));
}
