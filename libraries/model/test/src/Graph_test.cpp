//
// Graph tests
//

#include "Graph_test.h"

// model
#include "ModelGraph.h"
#include "ModelTransformer.h"
#include "ValueSelectorNode.h"
#include "ExtremalValueNode.h"
#include "InputNode.h"
#include "InputPort.h"
#include "OutputPort.h"

// nodes
#include "ConstantNode.h"
#include "MovingAverageNode.h"

// common
#include "LoadModelGraph.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <unordered_map>

void NodePrinter(const model::Node& node)
{
    bool isFirstInputPort = true;
    std::cout << "node_" << node.GetId() << " = " << node.GetRuntimeTypeName() << "(";
    for (const auto& inputPort : node.GetInputPorts())
    {
        std::cout << (isFirstInputPort ? "" : ", ");
        isFirstInputPort = false;

        auto ranges = inputPort->GetInputRanges();
        if(ranges.NumRanges() > 1)
        {
            std::cout << "{";
        }

        bool isFirstRange = true;
        for(const auto& range: ranges)
        {
            std::cout << (isFirstRange ? "" : ", ");
            isFirstRange = false;

            auto port = range.ReferencedPort();
            std::cout << "node_" << port->GetNode()->GetId() << "." << port->GetName();
            if(!range.IsFullPortRange())
            {
                auto start = range.GetStartIndex();
                auto size = range.Size();
                std::cout << "[" << start << ":" << (start+size) << "]";
            }
        }

        if(ranges.NumRanges() > 1)
        {
            std::cout << "}";
        }
    }
    std::cout << ")" << std::endl;
};

void PrintGraph(const model::Model& graph)
{
    graph.Visit(NodePrinter);
}

void PrintGraphIterator(const model::Model& graph)
{
    auto iter = graph.GetNodeIterator();
    while(iter.IsValid())
    {
        NodePrinter(*iter.Get());
        iter.Next();
    }
}

void PrintGraph(const model::Model& graph, const model::Node* output)
{
    graph.Visit(NodePrinter, output);
}

void TestStaticGraph()
{
    // Create a simple computation graph
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto maxAndArgMax = g.AddNode<model::ArgMaxNode<double>>(in->output);
    auto minAndArgMin = g.AddNode<model::ArgMinNode<double>>(in->output);
    auto condition = g.AddNode<nodes::ConstantNode<bool>>(true);
    auto valSelector = g.AddNode<model::ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);
    auto indexSelector = g.AddNode<model::ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);

    //
    // Print various subgraphs
    //
    std::cout << "\nFullGraph:" << std::endl;
    PrintGraph(g);

    std::cout << "\nGraph necessary for selected value:" << std::endl;
    PrintGraph(g, valSelector);

    std::cout << "\nGraph necessary for selected index:" << std::endl;
    PrintGraph(g, indexSelector);

    //
    // Compute outputs of various nodes
    //
    // Set the input node's current values
    std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    in->SetInput(inputValues);

    std::cout << "\nComputing output of Input node" << std::endl;
    auto output1 = g.ComputeNodeOutput(in->output);
    for (auto x : output1)
        std::cout << x << "  ";
    std::cout << std::endl;

    std::cout << "\nComputing output of condition node" << std::endl;
    auto conditionOutput = g.ComputeNodeOutput(condition->output);
    for (auto x : conditionOutput)
        std::cout << x << "  ";
    std::cout << std::endl;

    // std::cout << "\nComputing output of ArgMax node" << std::endl;
    // auto maxOutput = g.ComputeNodeOutput(maxAndArgMax->val);
    // for(auto x: maxOutput) std::cout << x << "  ";
    // std::cout << std::endl;

    std::cout << "\nComputing output of valSelector node" << std::endl;
    auto output3 = g.ComputeNodeOutput(valSelector->output);
    for (auto x : output3)
        std::cout << x << "  ";
    std::cout << std::endl;
    testing::ProcessTest("Testing min value", testing::IsEqual(output3[0], 0.75));

    std::cout << "\nComputing output of indexSelector node" << std::endl;
    auto output4 = g.ComputeNodeOutput(indexSelector->output);
    for (auto x : output4)
        std::cout << x << "  ";
    std::cout << std::endl;
    testing::ProcessTest("Testing min index", testing::IsEqual(output4[0], 2));
}

model::Model GetCompoundGraph()
{
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto minAndArgMin = g.AddNode<model::ArgMinNode<double>>(in->output);
    auto maxAndArgMax = g.AddNode<model::ArgMaxNode<double>>(in->output);
    auto meanMin = g.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 8);
    auto meanMax = g.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 8);
    return g;
}

void TestNodeIterator()
{
    auto model = GetCompoundGraph();
    auto size1 = model.Size();
    auto size2 = 0;
    auto iter = model.GetNodeIterator();
    while(iter.IsValid())
    {
        ++size2;
        iter.Next();
    }
    testing::ProcessTest("Testing Size() and iterator count", testing::IsEqual(size1, size2));
    testing::ProcessTest("Testing Size() and known node count", testing::IsEqual(size1, 5));

    std::cout << std::endl << std::endl;
}

void TestExampleGraph()
{
    auto model = common::LoadModelGraph("");
    PrintGraph(model);

    auto inputNodes = model.GetNodesByType<model::InputNode<double>>();
    std::cout << "# input nodes: " << inputNodes.size() << std::endl;
}

void TestInputRouting1()
{
    // Create a simple computation graph that computes both min and max and concatenates them
    // model::Model model;

    // auto in = model.AddNode<model::InputNode<double>>(3);

    // auto minAndArgMin = model.AddNode<model::ArgMinNode<double>>(in->output);
    // auto maxAndArgMax = model.AddNode<model::ArgMaxNode<double>>(in->output);
    // model::OutputPortRangeList ranges = { { minAndArgMin->val, 0}, {maxAndArgMax->val, 0} };
    // model::OutputPortRangeList ranges2 = { { minAndArgMin->val, 0}, {in->output, 1, 2} };

    // auto minAndMax = model.AddNode<model::CombinerNode<double>>(ranges);
    // auto minAndTail = model.AddNode<model::CombinerNode<double>>(ranges2);

    // set some example input and read the output
    // std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    // in->SetInput(inputValues);
    // auto output = model.ComputeNodeOutput(minAndMax->output);

    // testing::ProcessTest("Testing combine node", testing::IsEqual(output[0], 0.25));
    // testing::ProcessTest("Testing combine node", testing::IsEqual(output[1], 0.75));

    // auto output2 = model.ComputeNodeOutput(minAndTail->output);
    // std::cout << "size: " << output2.size() << std::endl;
    // for (auto val : output2) std::cout << val << "  ";
    // std::cout << std::endl;
}

void TestInputRouting2()
{
    // Create a simple computation graph that computes both min and max and concatenates them
    model::Model model;

    auto in = model.AddNode<model::InputNode<double>>(3);
    model::OutputPortElementList<double> range = { in->output, 0, 2 };
    model::OutputPortElementList<double> ranges = { { in->output, 0 }, { in->output, 2 } };

    auto minAndArgMin1 = model.AddNode<model::ArgMinNode<double>>(in->output); // a "standard" node that takes its input from an output port
    auto minAndArgMin2 = model.AddNode<model::ArgMinNode<double>>(range);      // a node that takes its input from a range --- a subset of outputs from a port
    auto minAndArgMin3 = model.AddNode<model::ArgMinNode<double>>(ranges);     // a node that takes its input from a "group" --- an arbitrary set of outputs from other ports

    auto minAndArgMin4 = model.AddNode<model::ArgMinNode<double>>(model::MakeOutputPortElementList(in->output, 0, 2));
    auto minAndArgMin5 = model.AddNode<model::ArgMinNode<double>>(model::OutputPortElementList<double>{ { in->output, 0 }, { in->output, 0, 2 } });
    auto minAndArgMin6 = model.AddNode<model::ArgMinNode<double>>(model::Concat(model::MakeOutputPortElementList(in->output, 0), model::MakeOutputPortElementList(in->output, 0, 2), model::MakeOutputPortElementList(minAndArgMin1->val, 0, 1)));

    //// set some example input and read the output
    std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    in->SetInput(inputValues);

    auto output1 = model.ComputeNodeOutput(minAndArgMin1->val);
    auto output2 = model.ComputeNodeOutput(minAndArgMin2->val);
    auto output3 = model.ComputeNodeOutput(minAndArgMin3->val);
    auto output4 = model.ComputeNodeOutput(minAndArgMin4->val);
    //    auto output5 = model.ComputeNodeOutput(minAndArgMin5->val);

    std::cout << "output1: " << output1[0] << ", output2: " << output2[0] << ", output3: " << output3[0] << ", output4: " << output4[0] << std::endl; // ", output5: " << output5[0] << std::endl;

    testing::ProcessTest("testing combine node", testing::IsEqual(output1[0], 0.25));
    testing::ProcessTest("testing combine node", testing::IsEqual(output2[0], 0.25));
    testing::ProcessTest("testing combine node", testing::IsEqual(output3[0], 0.5));
    testing::ProcessTest("testing combine node", testing::IsEqual(output4[0], output2[0]));
}

//
// Placeholder for test function that creates a graph using dynamic-creation routines
//

void TestCopyGraph()
{
    // Create a simple computation graph
    model::Model model;
    auto in = model.AddNode<model::InputNode<double>>(3);
    auto maxAndArgMax = model.AddNode<model::ArgMaxNode<double>>(in->output);
    auto minAndArgMin = model.AddNode<model::ArgMinNode<double>>(in->output);
    auto condition = model.AddNode<nodes::ConstantNode<bool>>(true);
    auto valSelector = model.AddNode<model::ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);
    auto indexSelector = model.AddNode<model::ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);

    // Now make a copy
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto newModel = transformer.CopyModel(model, context);

    // Print them both:
    std::cout << "\n\nOld graph" << std::endl;
    std::cout << "---------" << std::endl;
    PrintGraph(model);

    std::cout << "\n\nCopied graph" << std::endl;
    std::cout << "---------" << std::endl;
    PrintGraph(newModel);
}

void TestRefineGraph()
{
    // Create a simple computation graph
    model::Model model;

    auto inputNode = model.AddNode<model::InputNode<double>>(2);
    model::OutputPortElementList<double> inputValue = { inputNode->output, 0 };
    model::OutputPortElementList<double> inputThresh = { inputNode->output, 1 };

    auto value1 = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto value2 = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 100.0, 200.0, 300.0 });
    auto outputNode = model.AddNode<model::SelectIfLessNode<double>>(inputValue, inputThresh, value1->output, value2->output);

    // Now transform it
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto newModel = transformer.RefineModel(model, context);

    // Print both graphs
    std::cout << "\n\nOld graph" << std::endl;
    std::cout << "---------" << std::endl;
    PrintGraph(model);

    std::cout << "\n\nRefined graph" << std::endl;
    std::cout << "---------" << std::endl;
    PrintGraph(newModel);

    // Now run data through the graphs and make sure they agree
    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto newOutputPort = transformer.GetCorrespondingOutputPort(outputNode->output);

    std::vector<std::vector<double>> inputValues = { { 1.0, 2.0 }, { 1.0, 0.5 }, { 2.0, 4.0 } };
    for (const auto& inputValue : inputValues)
    {
        inputNode->SetInput(inputValue);
        auto output = model.ComputeNodeOutput(outputNode->output);

        newInputNode->SetInput(inputValue);
        auto newOutput = newModel.ComputeNodeOutput(*newOutputPort);

        testing::ProcessTest("testing refined graph", testing::IsEqual(output[0], newOutput[0]));
        testing::ProcessTest("testing refined graph", testing::IsEqual(output[1], newOutput[1]));
        testing::ProcessTest("testing refined graph", testing::IsEqual(output[2], newOutput[2]));
    }
}
