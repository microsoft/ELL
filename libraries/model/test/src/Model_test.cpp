////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Model_test.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Model_test.h"

#include <model_testing/include/ModelTestUtilities.h>

#include <common/include/LoadModel.h>

#include <model/include/InputNode.h>
#include <model/include/InputPort.h>
#include <model/include/Model.h>
#include <model/include/ModelEditor.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputNode.h>
#include <model/include/OutputPort.h>

#include <nodes/include/ConstantNode.h>
#include <nodes/include/DotProductNode.h>
#include <nodes/include/ExtremalValueNode.h>
#include <nodes/include/MovingAverageNode.h>
#include <nodes/include/ValueSelectorNode.h>

#include <testing/include/testing.h>

#include <utilities/include/Unused.h>

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

//
// Utility code
//

class NodeIdSet
{
public:
    bool Contains(const model::Node::NodeId& id) const
    {
        return _ids.find(id) != _ids.end();
    }

    void Add(const model::Node::NodeId& id)
    {
        _ids.insert(id);
    }

    size_t Size() const
    {
        return _ids.size();
    }

private:
    std::unordered_set<model::Node::NodeId> _ids;
};

int CountNodes(model::NodeIterator& iterator)
{
    int count = 0;
    while (iterator.IsValid())
    {
        ++count;
        iterator.Next();
    }
    return count;
}

// "two-output model":
// in -> argmin -> moving_avg
//   \-> argax -> moving_avg
model::Model GetTwoOutputModel()
{
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto minAndArgMin = g.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto maxAndArgMax = g.AddNode<nodes::ArgMaxNode<double>>(in->output);
    g.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 8);
    g.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 8);
    return g;
}

//
// The tests
//
void TestStaticModel()
{
    // Create a simple computation model
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto maxAndArgMax = g.AddNode<nodes::ArgMaxNode<double>>(in->output);
    auto minAndArgMin = g.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto condition = g.AddNode<nodes::ConstantNode<bool>>(true);
    auto valSelector = g.AddNode<nodes::ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);
    auto indexSelector = g.AddNode<nodes::ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);

    auto iter = g.GetNodeIterator();
    testing::ProcessTest("Testing full model size", testing::IsEqual(CountNodes(iter), 6));
    iter = g.GetNodeIterator(&valSelector->output);
    testing::ProcessTest("Testing partial model size 1", testing::IsEqual(CountNodes(iter), 5));
    iter = g.GetNodeIterator(&indexSelector->output);
    testing::ProcessTest("Testing partial model size 2", testing::IsEqual(CountNodes(iter), 5));

    //
    // Compute outputs of various nodes
    //

    // Set the input node's current values
    std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    in->SetInput(inputValues);

    auto output1 = g.ComputeOutput(in->output);
    testing::ProcessTest("Testing input node", testing::IsEqual(output1, inputValues));

    auto conditionOutput = g.ComputeOutput(condition->output);
    testing::ProcessTest("Testing condition node", testing::IsEqual(conditionOutput, std::vector<bool>{ true }));

    auto minOutput = g.ComputeOutput(minAndArgMin->val);
    testing::ProcessTest("Testing min value", testing::IsEqual(minOutput[0], 0.25));

    auto output3 = g.ComputeOutput(valSelector->output);
    testing::ProcessTest("Testing max value", testing::IsEqual(output3[0], 0.75));

    auto output4 = g.ComputeOutput(indexSelector->output);
    testing::ProcessTest("Testing max index", testing::IsEqual(output4[0], 2));
}

void TestNodeIterator()
{
    TestNodeIterator_Full();
    TestNodeIterator_Prefix();
    TestNodeIterator_Suffix();
    TestNodeIterator_Middle();
}

void TestNodeIterator_Full()
{
    auto model = GetTwoOutputModel();
    auto iter = model.GetNodeIterator();
    NodeIdSet visitedNodeIds;
    while (iter.IsValid())
    {
        auto node = iter.Get();
        for (auto parent : node->GetParentNodes())
        {
            testing::ProcessTest("Testing node's inputs already visited", visitedNodeIds.Contains(parent->GetId()));
        }
        for (auto dependent : node->GetDependentNodes())
        {
            testing::ProcessTest("Testing node's outputs not already visited", !visitedNodeIds.Contains(dependent->GetId()));
        }
        iter.Next();
        visitedNodeIds.Add(node->GetId());
    }
    testing::ProcessTest("Testing NodeIterator count over full model", model.Size() == visitedNodeIds.Size());
}

void TestNodeIterator_Prefix()
{
    // model: in -> d1 -> d2 -> d3 -> d4 -> d5
    auto model = GetLinearDebugNodeModel(5);
    NodeIdSet visitedNodeIds;
    auto lastNode = FindDebugNode(model, 3);
    testing::ProcessQuietTest("Found last node", lastNode != nullptr);
    size_t expectedPrefixSize = 4;
    auto iter = model.GetNodeIterator(&lastNode->output);
    while (iter.IsValid())
    {
        auto node = iter.Get();
        for (auto parent : node->GetParentNodes())
        {
            testing::ProcessTest("Testing node's inputs already visited", visitedNodeIds.Contains(parent->GetId()));
        }
        for (auto dependent : node->GetDependentNodes())
        {
            testing::ProcessTest("Testing node's outputs not already visited", !visitedNodeIds.Contains(dependent->GetId()));
        }
        iter.Next();
        visitedNodeIds.Add(node->GetId());
    }
    testing::ProcessTest("Testing NodeIterator count over model prefix", visitedNodeIds.Size() == expectedPrefixSize);
}

void TestNodeIterator_Suffix()
{
    // model: in -> d1 -> d2 -> d3 -> d4 -> d5
    auto model = GetLinearDebugNodeModel(5);
    NodeIdSet visitedNodeIds;
    auto firstNode = FindDebugNode(model, 3);
    testing::ProcessQuietTest("Found first node", firstNode != nullptr);
    const auto& firstInput = firstNode->input;
    size_t expectedSuffixSize = 3;
    auto iter = model.GetNodeIterator({ &firstInput }, {});
    while (iter.IsValid())
    {
        auto node = iter.Get();
        bool shouldHaveVisitedParent = (node != firstNode);
        for (auto parent : node->GetParentNodes())
        {
            testing::ProcessTest("Testing node's inputs already visited", visitedNodeIds.Contains(parent->GetId()) == shouldHaveVisitedParent);
        }
        for (auto dependent : node->GetDependentNodes())
        {
            testing::ProcessTest("Testing node's outputs not already visited", !visitedNodeIds.Contains(dependent->GetId()));
        }
        iter.Next();
        visitedNodeIds.Add(node->GetId());
        std::cout << node->GetRuntimeTypeName();
        auto debugNode = dynamic_cast<const DebugNode<double, int>*>(node);
        if (debugNode)
        {
            std::cout << " tag: " << debugNode->GetDebugInfo();
        }
        std::cout << std::endl;
    }

    auto success = testing::ProcessTest("Testing NodeIterator count over model suffix", visitedNodeIds.Size() == expectedSuffixSize);
    if (!success)
        std::cout << "Expected size: " << expectedSuffixSize << ", got: " << visitedNodeIds.Size() << std::endl;
}

void TestNodeIterator_Middle()
{
    // model: in -> d1 -> d2 -> d3 -> d4 -> d5
    auto model = GetLinearDebugNodeModel(5);
    NodeIdSet visitedNodeIds;
    auto firstNode = FindDebugNode(model, 2);
    testing::ProcessQuietTest("Found first node", firstNode != nullptr);
    const auto& firstInput = firstNode->input;
    auto lastNode = FindDebugNode(model, 4);
    testing::ProcessQuietTest("Found last node", lastNode != nullptr);
    const auto& lastOutput = lastNode->output;

    size_t expectedSubmodelSize = 3;
    auto iter = model.GetNodeIterator({ &firstInput }, { &lastOutput });
    while (iter.IsValid())
    {
        auto node = iter.Get();
        bool shouldHaveVisitedParent = (node != firstNode);
        for (auto parent : node->GetParentNodes())
        {
            testing::ProcessTest("Testing node's inputs already visited", visitedNodeIds.Contains(parent->GetId()) == shouldHaveVisitedParent);
        }

        for (auto dependent : node->GetDependentNodes())
        {
            testing::ProcessTest("Testing node's outputs not already visited", !visitedNodeIds.Contains(dependent->GetId()));
        }
        iter.Next();
        visitedNodeIds.Add(node->GetId());
    }

    auto success = testing::ProcessTest("Testing NodeIterator count over model subgraph", visitedNodeIds.Size() == expectedSubmodelSize);
    if (!success)
        std::cout << "Expected size: " << expectedSubmodelSize << ", got: " << visitedNodeIds.Size() << std::endl;
}

void TestReverseNodeIterator()
{
    auto model = GetTwoOutputModel();
    auto iter = model.GetReverseNodeIterator();
    NodeIdSet visitedNodeIds;
    while (iter.IsValid())
    {
        const model::Node* node = iter.Get();
        for (const model::Node* parent : node->GetParentNodes())
        {
            testing::ProcessTest("Testing node's inputs not already visited", !visitedNodeIds.Contains(parent->GetId()));
        }
        for (const model::Node* dependent : node->GetDependentNodes())
        {
            testing::ProcessTest("Testing node's outputs already visited", visitedNodeIds.Contains(dependent->GetId()));
        }
        iter.Next();
        visitedNodeIds.Add(node->GetId());
    }
    testing::ProcessTest("Testing Size() and reverse iterator count", model.Size() == visitedNodeIds.Size());
}

void TestModelSerialization()
{
    auto model1 = GetTwoOutputModel();
    std::stringstream buffer;
    utilities::JsonArchiver archiver(buffer);
    archiver << model1;

    // Now unarchive model
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    utilities::JsonUnarchiver unarchiver(buffer, context);
    model::Model model2;
    unarchiver >> model2;

    testing::ProcessTest("Testing model serialization", testing::IsEqual(model1.Size(), model2.Size()));
}

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

    // Test metadata survives a model copy via ModelTransformer
    ell::model::ModelTransformer t;
    auto copy = t.CopyModel(model);
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

void TestInputRouting()
{
    // Create a simple computation model that computes both min and max and concatenates them
    model::Model model;

    auto in = model.AddNode<model::InputNode<double>>(3);
    model::PortElements<double> range = { in->output, 0, 2 };
    model::PortElements<double> ranges = { { in->output, 0 }, { in->output, 2 } };

    auto minAndArgMin1 = model.AddNode<nodes::ArgMinNode<double>>(in->output); // a "standard" node that takes its input from an output port
    auto minAndArgMin2 = model.AddNode<nodes::ArgMinNode<double>>(range); // a node that takes its input from a range --- a subset of outputs from a port
    auto minAndArgMin3 = model.AddNode<nodes::ArgMinNode<double>>(ranges); // a node that takes its input from a "group" --- an arbitrary set of outputs from other ports

    auto minAndArgMin4 = model.AddNode<nodes::ArgMinNode<double>>(model::PortElements<double>(in->output, 0, 2));
    model.AddNode<nodes::ArgMinNode<double>>(model::PortElements<double>{ { in->output, 0 }, { in->output, 0, 2 } });
    model.AddNode<nodes::ArgMinNode<double>>(model::PortElements<double>{ { in->output, 0 }, { in->output, 0, 2 }, { minAndArgMin1->val, 0, 1 } });

    //// set some example input and read the output
    std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    in->SetInput(inputValues);

    auto output1 = model.ComputeOutput(minAndArgMin1->val);
    auto output2 = model.ComputeOutput(minAndArgMin2->val);
    auto output3 = model.ComputeOutput(minAndArgMin3->val);
    auto output4 = model.ComputeOutput(minAndArgMin4->val);

    testing::ProcessTest("testing combine node", testing::IsEqual(output1[0], 0.25));
    testing::ProcessTest("testing combine node", testing::IsEqual(output2[0], 0.25));
    testing::ProcessTest("testing combine node", testing::IsEqual(output3[0], 0.5));
    testing::ProcessTest("testing combine node", testing::IsEqual(output4[0], output2[0]));
}

//
// Placeholder for test function that creates a model using dynamic-creation routines
//

void TestDenseCopyModel()
{
    // Create a simple computation model
    model::Model model;
    auto in = model.AddNode<model::InputNode<double>>(3);
    auto maxAndArgMax = model.AddNode<nodes::ArgMaxNode<double>>(in->output);
    auto minAndArgMin = model.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto condition = model.AddNode<nodes::ConstantNode<bool>>(true);
    model.AddNode<nodes::ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);
    model.AddNode<nodes::ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);

    // Now make a copy
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto copiedModel = transformer.CopyModel(model, context);

    // make sure they're the same
    auto originalModelIterator = model.GetNodeIterator();
    auto copiedModelIterator = copiedModel.GetNodeIterator();

    std::unordered_map<std::string, int> originalNodeCounts;
    std::unordered_map<std::string, int> copiedNodeCounts;
    while (originalModelIterator.IsValid())
    {
        auto originalNode = originalModelIterator.Get();
        originalNodeCounts[originalNode->GetRuntimeTypeName()] += 1;
        auto copiedNode = copiedModelIterator.Get();
        copiedNodeCounts[copiedNode->GetRuntimeTypeName()] += 1;
        originalModelIterator.Next();
        copiedModelIterator.Next();
    }
    // Check that both iterators ran out at the same time
    testing::ProcessTest("testing model copy", !copiedModelIterator.IsValid());

    for (auto originalIter : originalNodeCounts)
    {
        testing::ProcessTest("testing model copy", originalIter.second == copiedNodeCounts[originalIter.first]);
    }

    // add a node to the first model
    model.AddNode<model::OutputNode<double>>(minAndArgMin->val);

    // make sure second one is different
    testing::ProcessTest("testing model copy", model.Size() == copiedModel.Size() + 1);
}

void TestShallowCopyModel()
{
    // Create a simple computation model
    model::Model model;
    auto in = model.AddNode<model::InputNode<double>>(3);
    auto maxAndArgMax = model.AddNode<nodes::ArgMaxNode<double>>(in->output);
    auto minAndArgMin = model.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto condition = model.AddNode<nodes::ConstantNode<bool>>(true);
    model.AddNode<nodes::ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);
    model.AddNode<nodes::ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);

    // Now make a shallow copy
    auto copiedModel = model.ShallowCopy();

    // make sure they're the same
    auto originalModelIterator = model.GetNodeIterator();
    auto copiedModelIterator = copiedModel.GetNodeIterator();

    std::unordered_map<std::string, int> originalNodeCounts;
    std::unordered_map<std::string, int> copiedNodeCounts;
    while (originalModelIterator.IsValid())
    {
        auto originalNode = originalModelIterator.Get();
        originalNodeCounts[originalNode->GetRuntimeTypeName()] += 1;
        auto copiedNode = copiedModelIterator.Get();
        copiedNodeCounts[copiedNode->GetRuntimeTypeName()] += 1;
        originalModelIterator.Next();
        copiedModelIterator.Next();
    }
    // Check that both iterators ran out at the same time
    testing::ProcessTest("testing model copy", !copiedModelIterator.IsValid());

    for (auto originalIter : originalNodeCounts)
    {
        testing::ProcessTest("testing model copy", originalIter.second == copiedNodeCounts[originalIter.first]);
    }

    // add a node to the first model
    model.AddNode<model::OutputNode<double>>(minAndArgMin->val);

    // make sure second one is the same
    testing::ProcessTest("testing model copy", model.Size() == copiedModel.Size());
}

// Define new node that splits its outputs when refined
template <typename ValueType>
class SplittingNode : public model::Node
{
public:
    SplittingNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, inputPortName),
        _output(this, outputPortName, 0){};
    SplittingNode(const model::OutputPort<ValueType>& input) :
        Node({ &_input }, { &_output }),
        _input(this, input, inputPortName),
        _output(this, outputPortName, input.Size()){};

    static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SplittingNode"); }
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    void Copy(model::ModelTransformer& transformer) const override
    {
        const auto& newInput = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<SplittingNode<ValueType>>(newInput);
        transformer.MapNodeOutput(output, newNode->output);
    }

    bool Refine(model::ModelTransformer& transformer) const override
    {
        auto newInput = model::PortElements<ValueType>{ transformer.GetCorrespondingInputs(_input) };
        model::PortElements<ValueType> in1;
        model::PortElements<ValueType> in2;
        auto size = _input.Size();
        auto halfSize = size / 2;

        // split into two nodes, one which returns the first half, and one which returns the second half
        for (size_t index = 0; index < halfSize; ++index)
        {
            in1.Append(model::PortElements<ValueType>(newInput.GetElement(index)));
        }
        for (size_t index = halfSize; index < size; ++index)
        {
            in2.Append(model::PortElements<ValueType>(newInput.GetElement(index)));
        }
        auto newNode1 = transformer.AddNode<model::OutputNode<ValueType>>(in1);
        auto newNode2 = transformer.AddNode<model::OutputNode<ValueType>>(in2);
        model::PortElements<ValueType> elem1(newNode1->output);
        model::PortElements<ValueType> elem2(newNode2->output);
        model::PortElements<ValueType> newOutput({ elem1, elem2 });

        transformer.MapNodeOutput(output, newOutput);
        return true;
    }

    const model::OutputPort<ValueType>& output = _output;
    static constexpr const char* inputPortName = "input";
    static constexpr const char* outputPortName = "output";

    void WriteToArchive(utilities::Archiver& archiver) const override
    {
        archiver["input"] << _input;
        archiver["output"] << _output;
    }

    void ReadFromArchive(utilities::Unarchiver& archiver) override
    {
        archiver["input"] >> _input;
        archiver["output"] >> _output;
    }

protected:
    void Compute() const override { _output.SetOutput(_input.GetValue()); }

private:
    model::InputPort<ValueType> _input;
    model::OutputPort<ValueType> _output;
};

void TestRefineSplitOutputs()
{
    // Create a simple computation model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(2);
    auto outputNode = model.AddNode<SplittingNode<double>>(inputNode->output);

    model::TransformContext context;
    model::ModelTransformer transformer;
    auto newModel = transformer.RefineModel(model, context);

    // Now run data through the models and make sure they agree
    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    const auto& newOutputs = transformer.GetCorrespondingOutputs(model::PortElements<double>{ outputNode->output });

    std::vector<std::vector<double>> inputValues = { { 1.0, 2.0 }, { 1.0, 0.5 }, { 2.0, 4.0 } };
    for (const auto& inputValue : inputValues)
    {
        inputNode->SetInput(inputValue);
        auto output = model.ComputeOutput(outputNode->output);

        newInputNode->SetInput(inputValue);
        auto newOutput = newModel.ComputeOutput(newOutputs);

        testing::ProcessTest("testing refined splitting model", testing::IsEqual(output[0], newOutput[0]));
        testing::ProcessTest("testing refined splitting model", testing::IsEqual(output[1], newOutput[1]));
    }
}

void TestCustomRefine()
{
    // Create a simple computation model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(2);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0 });
    model.AddNode<nodes::DotProductNode<double>>(inputNode->output, constantNode->output);

    model::ModelTransformer transformer;
    model::TransformContext context1;
    context1.AddNodeActionFunction([](const model::Node& node) { return dynamic_cast<const nodes::DotProductNode<double>*>(&node) == nullptr ? model::NodeAction::abstain : model::NodeAction::refine; });
    auto model1 = transformer.RefineModel(model, context1);

    model::TransformContext context2;
    context2.AddNodeActionFunction([](const model::Node& node) { return dynamic_cast<const nodes::DotProductNode<double>*>(&node) == nullptr ? model::NodeAction::abstain : model::NodeAction::compile; });
    auto model2 = transformer.RefineModel(model, context2);
    testing::ProcessTest("testing custom refine function", model1.Size() == 4 && model2.Size() == 3);
}

void TestChangeInputForNode()
{
    // Create a simple computation model
    model::Model model;
    auto in = model.AddNode<model::InputNode<double>>(3);
    auto maxAndArgMax = model.AddNode<nodes::ArgMaxNode<double>>(in->output);
    auto minAndArgMin = model.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto trueCondition = model.AddNode<nodes::ConstantNode<bool>>(true);
    model.AddNode<nodes::ValueSelectorNode<double>>(trueCondition->output, maxAndArgMax->val, minAndArgMin->val);
    model.AddNode<nodes::ValueSelectorNode<int>>(trueCondition->output, maxAndArgMax->argVal, minAndArgMin->argVal);

    // Now make a copy
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto newModel = transformer.CopyModel(model, context);

    // Print them both:
    std::cout << "\n\nOld model" << std::endl;
    std::cout << "---------" << std::endl;
    PrintModel(model);

    nodes::ValueSelectorNode<double>* doubleSelectorNode = nullptr;
    {
        auto nodes = newModel.GetNodesByType<nodes::ValueSelectorNode<double>>();
        testing::IsTrue(nodes.size() == 1u);
        doubleSelectorNode = nodes[0];
    }
    nodes::ValueSelectorNode<int>* intSelectorNode = nullptr;
    {
        auto nodes = newModel.GetNodesByType<nodes::ValueSelectorNode<int>>();
        testing::IsTrue(nodes.size() == 1u);
        intSelectorNode = nodes[0];
    }

    {
        auto doubleSelectorNodeConditionInputNodes = doubleSelectorNode->condition.GetParentNodes();
        auto intSelectorNodeConditionInputNodes = intSelectorNode->condition.GetParentNodes();
        testing::IsTrue(doubleSelectorNodeConditionInputNodes.size() == 1u && intSelectorNodeConditionInputNodes.size() == 1u);
        testing::IsTrue(doubleSelectorNodeConditionInputNodes[0]->GetId() == intSelectorNodeConditionInputNodes[0]->GetId());
    }

    auto falseCondition = newModel.AddNode<nodes::ConstantNode<bool>>(true);
    model::ModelEditor::ResetInputPort(&doubleSelectorNode->condition, falseCondition->output);
    {
        auto doubleSelectorNodeConditionInputNodes = doubleSelectorNode->condition.GetParentNodes();
        testing::IsTrue(doubleSelectorNodeConditionInputNodes.size() == 1u);
        auto conditionNode = doubleSelectorNodeConditionInputNodes[0];
        testing::IsTrue(conditionNode->GetId() == falseCondition->GetId());
    }

    std::cout << "\n\nCopied model" << std::endl;
    std::cout << "---------" << std::endl;
    PrintModel(newModel);

    std::cout << "\n\n"
              << std::endl;
}
