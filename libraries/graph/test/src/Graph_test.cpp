//
// Graph tests
// 

#include "Graph_test.h"

#include "DirectedGraph.h"
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

void NodePrinter(const Node& node)
{
    bool first = true;
    std::cout << "node_" << node.Id() << " = " << node.GetRuntimeTypeName() << "(";
    for(const auto& input: node.GetInputs())
    {
        std::cout << (first ? "" : ", ");
        first = false;
        std::cout << "node_" << input->Node()->Id() << "[" << input->Index() << "]";
        
    }
    std::cout << ")" << std::endl;        
};


void PrintGraph(const Model& graph)
{
    graph.Visit(NodePrinter);    
}

void PrintGraph(const Model& graph, const std::shared_ptr<Node>& output)
{    
    graph.Visit(NodePrinter, output);    
}

void TestStaticGraph()
{
    // Create a simple computation graph
    Model g;
    auto in = g.AddNode<InputNode<double>>(3);
    auto maxAndArgMax = g.AddNode<ArgMaxNode<double>>(in->output);
    auto minAndArgMin = g.AddNode<ArgMinNode<double>>(in->output);
    auto condition = g.AddNode<ConstantNode<bool>>(true);
    auto valSelector = g.AddNode<ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);
    auto indexSelector = g.AddNode<ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);

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

    // std::cout << "\nComputing output of ArgMax node" << std::endl;
    // auto output2 = g.GetNodeOutput(maxAndArgMax->val);
    // for(auto x: output2) std::cout << x << "  ";
    // std::cout << std::endl;

    std::cout << "\nComputing output of valSelector node" << std::endl;
    auto output3 = g.GetNodeOutput(valSelector->output);
    for (auto x : output3) std::cout << x << "  ";
    std::cout << std::endl;
    testing::ProcessTest("Testing min value", testing::IsEqual(output3[0], 0.25));

    std::cout << "\nComputing output of indexSelector node" << std::endl;
    auto output4 = g.GetNodeOutput(indexSelector->output);
    for (auto x : output4) std::cout << x << "  ";
    std::cout << std::endl;
    testing::ProcessTest("Testing min index", testing::IsEqual(output4[0], 1));
}

//
// Placeholder for test function that creates a graph using dynamic-creation routines
// 
void TestDynamicGraph()
{
    // Create a simple computation graph
    Model model;

    auto in = model.AddNode<InputNode<double>>(3);
    auto maxAndArgMax = model.AddNode<ArgMaxNode<double>>(in->output);
    auto minAndArgMin = model.AddNode<ArgMinNode<double>>(in->output);
    auto condition = model.AddNode<ConstantNode<bool>>(true);
    auto valSelector = model.AddNode<ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);
    auto indexSelector = model.AddNode<ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);

}