////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Model_test.cpp (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Model_test.h"
#include "ModelTestUtilities.h"

// common
#include "LoadModel.h"

// model
#include "InputNode.h"
#include "InputPort.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "OutputNode.h"
#include "OutputPort.h"

// nodes
#include "ConstantNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "MovingAverageNode.h"
#include "ValueSelectorNode.h"

// testing
#include "testing.h"

// stl
#include <iomanip>
#include <iostream>
#include <unordered_map>

using namespace ell;

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

    //
    // Print various submodels
    //
    std::cout << "\nFullModel:" << std::endl;
    PrintModel(g);

    std::cout << "\nModel necessary for selected value:" << std::endl;
    PrintModel(g, valSelector);

    std::cout << "\nModel necessary for selected index:" << std::endl;
    PrintModel(g, indexSelector);

    //
    // Compute outputs of various nodes
    //
    // Set the input node's current values
    std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    in->SetInput(inputValues);

    std::cout << "\nComputing output of Input node" << std::endl;
    auto output1 = g.ComputeOutput(in->output);
    for (auto x : output1)
        std::cout << x << "  ";
    std::cout << std::endl;

    std::cout << "\nComputing output of condition node" << std::endl;
    auto conditionOutput = g.ComputeOutput(condition->output);
    for (auto x : conditionOutput)
        std::cout << x << "  ";
    std::cout << std::endl;

    // std::cout << "\nComputing output of ArgMax node" << std::endl;
    // auto maxOutput = g.ComputeOutput(maxAndArgMax->val);
    // for(auto x: maxOutput) std::cout << x << "  ";
    // std::cout << std::endl;

    std::cout << "\nComputing output of valSelector node" << std::endl;
    auto output3 = g.ComputeOutput(valSelector->output);
    for (auto x : output3)
        std::cout << x << "  ";
    std::cout << std::endl;
    testing::ProcessTest("Testing min value", testing::IsEqual(output3[0], 0.75));

    std::cout << "\nComputing output of indexSelector node" << std::endl;
    auto output4 = g.ComputeOutput(indexSelector->output);
    for (auto x : output4)
        std::cout << x << "  ";
    std::cout << std::endl;
    testing::ProcessTest("Testing min index", testing::IsEqual(output4[0], 2));
}

model::Model GetCompoundModel()
{
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto minAndArgMin = g.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto maxAndArgMax = g.AddNode<nodes::ArgMaxNode<double>>(in->output);
    g.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 8);
    g.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 8);
    return g;
}

void TestNodeIterator()
{
    auto model = GetCompoundModel();
    auto size1 = model.Size();
    size_t size2 = 0;
    auto iter = model.GetNodeIterator();
    while (iter.IsValid())
    {
        ++size2;
        iter.Next();
    }
    testing::ProcessTest("Testing Size() and iterator count", size1 == size2);
    testing::ProcessTest("Testing Size() and known node count", size1 == 5);

    std::cout << std::endl
              << std::endl;
}

void TestModelSerialization()
{
    auto model1 = GetCompoundModel();
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
    auto model = GetCompoundModel();
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

    // Print archive of model:
    std::cout << "Model with metadata:" << std::endl;
    utilities::JsonArchiver printArchiver(std::cout);
    printArchiver << model;

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

void TestInputRouting1()
{
    // Create a simple model that computes both min and max and concatenates them
    // model::Model model;

    // auto in = model.AddNode<model::InputNode<double>>(3);

    // auto minAndArgMin = model.AddNode<nodes::ArgMinNode<double>>(in->output);
    // auto maxAndArgMax = model.AddNode<nodes::ArgMaxNode<double>>(in->output);
    // model::PortRangeList ranges = { { minAndArgMin->val, 0}, {maxAndArgMax->val, 0} };
    // model::PortRangeList ranges2 = { { minAndArgMin->val, 0}, {in->output, 1, 2} };

    // auto minAndMax = model.AddNode<model::CombinerNode<double>>(ranges);
    // auto minAndTail = model.AddNode<model::CombinerNode<double>>(ranges2);

    // set some example input and read the output
    // std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    // in->SetInput(inputValues);
    // auto output = model.ComputeOutput(minAndMax->output);

    // testing::ProcessTest("Testing combine node", testing::IsEqual(output[0], 0.25));
    // testing::ProcessTest("Testing combine node", testing::IsEqual(output[1], 0.75));

    // auto output2 = model.ComputeOutput(minAndTail->output);
    // std::cout << "size: " << output2.size() << std::endl;
    // for (auto val : output2) std::cout << val << "  ";
    // std::cout << std::endl;
}

void TestInputRouting2()
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

void TestCopyModel()
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
    auto newModel = transformer.CopyModel(model, context);

    // Print them both:
    std::cout << "\n\nOld model" << std::endl;
    std::cout << "---------" << std::endl;
    PrintModel(model);

    std::cout << "\n\nCopied model" << std::endl;
    std::cout << "---------" << std::endl;
    PrintModel(newModel);

    std::cout << "\n\n"
              << std::endl;
}

// Define new node that splits its outputs when refined
template <typename ValueType>
class SplittingNode : public model::Node
{
public:
    SplittingNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0){};
    SplittingNode(const model::PortElements<ValueType>& input)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, input.Size()){};

    static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SplittingNode"); }
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    void Copy(model::ModelTransformer& transformer) const override
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SplittingNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    bool Refine(model::ModelTransformer& transformer) const override
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        model::PortElements<ValueType> in1;
        model::PortElements<ValueType> in2;
        auto size = _input.Size();
        auto halfSize = size / 2;
        // split into two nodes, one which returns the first half, and one which returns the second half
        for (size_t index = 0; index < halfSize; ++index)
        {
            in1.Append(model::PortElements<ValueType>(newPortElements.GetElement(index)));
        }
        for (size_t index = halfSize; index < size; ++index)
        {
            in2.Append(model::PortElements<ValueType>(newPortElements.GetElement(index)));
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
    auto newOutputs = transformer.GetCorrespondingOutputs(model::PortElements<double>{ outputNode->output });

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
