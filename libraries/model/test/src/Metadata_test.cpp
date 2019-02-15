////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Metadata_test.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Metadata_test.h"

#include <common/include/LoadModel.h>

#include <model_testing/include/ModelTestUtilities.h>

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <nodes/include/MovingAverageNode.h>

#include <testing/include/testing.h>

#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace ell;

// Prototypes
void TestNodeIterator_Full();
void TestNodeIterator_Prefix();
void TestNodeIterator_Suffix();
void TestNodeIterator_Middle();

// The tests
void TestModelMetadata()
{
    auto model = GetTwoOutputModel();
    auto iter = model.GetNodeIterator();
    while (iter.IsValid())
    {
        auto node = const_cast<model::Node*>(iter.Get());
        auto typeName = node->GetRuntimeTypeName();
        node->GetMetadata().SetEntry("visited", std::string("true"));
        node->GetMetadata().SetEntry("typeName", std::string(typeName));
        node->GetMetadata().SetEntry("foo", std::string("bar"));
        node->GetMetadata().SetEntry("foo", std::string("baz"));
        iter.Next();
    }

    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    for (auto inputNode : inputNodes)
    {
        auto node = const_cast<model::InputNode<double>*>(inputNode);
        node->GetMetadata().SetEntry("isInput", std::string("true"));
    }

    // Test copy of metadata
    auto inputNode = inputNodes[0];
    auto newNode = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
    newNode->GetMetadata() = inputNode->GetMetadata();
    testing::ProcessTest("Testing metadata copy", !newNode->GetMetadata().IsEmpty());

    // Test metadata survives a model copy via DeepCopy
    auto copy = model.DeepCopy();
    auto iter2 = copy.GetNodeIterator();
    while (iter2.IsValid())
    {
        auto node = const_cast<model::Node*>(iter2.Get());
        auto name = node->GetRuntimeTypeName();
        testing::ProcessTest("Testing metadata copy " + name + "::HasEntry('visited')", node->GetMetadata().HasEntry("visited"));
        testing::ProcessTest("Testing metadata copy " + name + "::GetEntry('visited') == 'true'", node->GetMetadata().GetEntry<std::string>("visited") == "true");
        testing::ProcessTest("Testing metadata copy " + name + "::HasEntry('foo')", node->GetMetadata().HasEntry("foo"));
        testing::ProcessTest("Testing metadata copy " + name + "::GetEntry('foo') == 'baz'", node->GetMetadata().GetEntry<std::string>("foo") == "baz");
        iter2.Next();
    }

    // Print archive of model:
#if 0
    std::cout << "Model with metadata:" << std::endl;
    utilities::JsonArchiver printArchiver(std::cout);
    printArchiver << model;
#endif

    std::stringstream buffer;
    utilities::JsonArchiver archiver(buffer);
    archiver << model;

    // Now unarchive model
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    utilities::JsonUnarchiver unarchiver(buffer, context);
    model::Model model2;
    unarchiver >> model2;
    auto inputNodes2 = model2.GetNodesByType<model::InputNode<double>>();
    for (auto inputNode : inputNodes2)
    {
        testing::ProcessTest("Testing metadata unarchiving", inputNode->GetMetadata().HasEntry("isInput"));
        testing::ProcessTest("Testing metadata unarchiving", inputNode->GetMetadata().GetEntry<std::string>("isInput") == "true");
    }
}
