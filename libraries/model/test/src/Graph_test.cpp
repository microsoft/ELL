//
// Graph tests
// 

#include "Graph_test.h"

#include "ModelGraph.h"
#include "ValueSelectorNode.h"
#include "ExtremalValueNode.h"
#include "ConstantNode.h"
#include "InputNode.h"
#include "InputPort.h"
#include "OutputPort.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <unordered_map>

// template <typename... Args>
// auto concat(Args... args)
// {
//     return { args... };
// }

void
NodePrinter(const model::Node& node)
{
    bool first = true;
    std::cout << "node_" << node.GetId() << " = " << node.GetRuntimeTypeName() << "(";
    for(const auto& input: node.GetInputs())
    {
        std::cout << (first ? "" : ", ");
        first = false;
//        std::cout << "node_" << input->ReferencedPort()->Node()->GetId() << "[" << input->ReferencedPort()->Index() << "]";
        
    }
    std::cout << ")" << std::endl;        
};

void
PrintGraph(const model::Model& graph)
{
    graph.Visit(NodePrinter);    
}

void PrintGraph(const model::Model& graph, const std::shared_ptr<model::Node>& output)
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
    auto condition = g.AddNode<model::ConstantNode<bool>>(true);
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
    std::vector<double> inputValues = {0.5, 0.25, 0.75};
    in->SetInput(inputValues);

    std::cout << "\nComputing output of Input node" << std::endl;
    auto output1 = g.GetNodeOutput(in->output);
    for (auto x : output1) std::cout << x << "  ";
    std::cout << std::endl;

    std::cout << "\nComputing output of condition node" << std::endl;
    auto conditionOutput = g.GetNodeOutput(condition->output);
    for (auto x : conditionOutput) std::cout << x << "  ";
    std::cout << std::endl;

     //std::cout << "\nComputing output of ArgMax node" << std::endl;
     //auto maxOutput = g.GetNodeOutput(maxAndArgMax->val);
     //for(auto x: maxOutput) std::cout << x << "  ";
     //std::cout << std::endl;

    std::cout << "\nComputing output of valSelector node" << std::endl;
    auto output3 = g.GetNodeOutput(valSelector->output);
    for (auto x : output3) std::cout << x << "  ";
    std::cout << std::endl;
    testing::ProcessTest("Testing min value", testing::IsEqual(output3[0], 0.75));

    std::cout << "\nComputing output of indexSelector node" << std::endl;
    auto output4 = g.GetNodeOutput(indexSelector->output);
    for (auto x : output4) std::cout << x << "  ";
    std::cout << std::endl;
    testing::ProcessTest("Testing min index", testing::IsEqual(output4[0], 2));
}

void TestInputRouting1()
{
    // Create a simple computation graph that computes both min and max and concatenates them
    //model::Model model;

    //auto in = model.AddNode<model::InputNode<double>>(3);

    //auto minAndArgMin = model.AddNode<model::ArgMinNode<double>>(in->output);
    //auto maxAndArgMax = model.AddNode<model::ArgMaxNode<double>>(in->output);
    //model::InputRangeList ranges = { { minAndArgMin->val, 0}, {maxAndArgMax->val, 0} };    
    //model::InputRangeList ranges2 = { { minAndArgMin->val, 0}, {in->output, 1, 2} };

    //auto minAndMax = model.AddNode<model::CombinerNode<double>>(ranges);
    //auto minAndTail = model.AddNode<model::CombinerNode<double>>(ranges2);

    // set some example input and read the output
    //std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    //in->SetInput(inputValues);
    //auto output = model.GetNodeOutput(minAndMax->output);

    //testing::ProcessTest("Testing combine node", testing::IsEqual(output[0], 0.25));
    //testing::ProcessTest("Testing combine node", testing::IsEqual(output[1], 0.75));

    //auto output2 = model.GetNodeOutput(minAndTail->output);
    //std::cout << "size: " << output2.size() << std::endl;
    //for (auto val : output2) std::cout << val << "  ";
    //std::cout << std::endl;
}

void TestInputRouting2()
{
    // Create a simple computation graph that computes both min and max and concatenates them
    model::Model model;

    auto in = model.AddNode<model::InputNode<double>>(3);
    model::TypedInputGroup<double> range(in->output, 0, 2);
    model::TypedInputGroup<double> ranges = { { in->output, 0}, {in->output, 2} };

    auto minAndArgMin1 = model.AddNode<model::ArgMinNode<double>>(in->output);
    auto minAndArgMin2 = model.AddNode<model::ArgMinNode<double>>(range);
    auto minAndArgMin3 = model.AddNode<model::ArgMinNode<double>>(ranges);

    auto minAndArgMin4 = model.AddNode<model::ArgMinNode<double>>(model::TypedInputGroup<double>{in->output, 0, 1});

    //// set some example input and read the output
    std::vector<double> inputValues = { 0.5, 0.25, 0.75 };
    in->SetInput(inputValues);

    auto output1 = model.GetNodeOutput(minAndArgMin1->val);
    auto output2 = model.GetNodeOutput(minAndArgMin2->val);
    auto output3 = model.GetNodeOutput(minAndArgMin3->val);

    testing::ProcessTest("testing combine node", testing::IsEqual(output1[0], 0.25));
    testing::ProcessTest("testing combine node", testing::IsEqual(output2[0], 0.25));
    testing::ProcessTest("testing combine node", testing::IsEqual(output3[0], 0.5));
}

//
// Placeholder for test function that creates a graph using dynamic-creation routines
// 

void TestDynamicGraph()
{
    // Create a simple computation graph
    model::Model model;

    auto in = model.AddNode<model::InputNode<double>>(3);
    auto maxAndArgMax = model.AddNode<model::ArgMaxNode<double>>(in->output);
    auto minAndArgMin = model.AddNode<model::ArgMinNode<double>>(in->output);
    auto condition = model.AddNode<model::ConstantNode<bool>>(true);
    auto valSelector = model.AddNode<model::ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);
    auto indexSelector = model.AddNode<model::ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);
}