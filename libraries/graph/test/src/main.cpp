//
// Graph tests
// 

#include "DirectedGraph.h"
#include "ValueSelectorNode.h"
#include "ExtremalValueNode.h"
#include "ConstantNode.h"
#include "InputNode.h"
#include "NodeInput.h"
#include "NodeOutput.h"

#include <iostream>
#include <unordered_map>

// template <typename... Args>
// auto concat(Args... args)
// {
//     return { args... };
// }

void PrintVisitor(const Node& node)
{
    bool first = true;
    std::cout << "node_" << node.Id() << " = " << node.NodeType() << "(";
    for(const auto& input: node.GetInputs())
    {
        std::cout << (first ? "" : ", ");
        first = false;
        std::cout << "node_" << input->GetNode()->Id() << "[" << input->GetOutputIndex() << "]";
        
    }
    std::cout << ")" << std::endl;        
};


void PrintGraph(const DirectedGraph& graph)
{
    graph.Visit(PrintVisitor);    
}

void PrintGraph(const DirectedGraph& graph, const std::shared_ptr<Node>& output)
{    
    graph.Visit(PrintVisitor, output);    
}

void CopyGraph(const DirectedGraph& graph1, const std::shared_ptr<Node>& output,  DirectedGraph& graph2)
{
    std::unordered_map<Node*, Node*> oldToNewMap;
    auto visitor = [&graph1, &graph2, &oldToNewMap](const Node& node)
    {
        auto inputs = node.GetInputs();
        std::vector<NodeInput> newInputs;
        for(const auto& oldInput: inputs) 
        {
            auto newInput = oldInput; // copy c'tor 
            // auto newInput = oldInput.CloneButNullifyNodePointers();
            // TODO: replace the _node field in the copy with oldToNewMap[oldInput.GetNode()]
            // nowInputs.push_back(newInput);
        }
        // auto newNode = graph2.MakeNodeByCloningNodeAndReplacingInputs(node, newInputs)
        // oldToNewMap[&node] = newNode;
    };
    
    graph1.Visit(visitor, output);
}


int main(int argc, char** argv)
{
    // Create a graph

    DirectedGraph g;
    auto in = g.AddNode<InputNode<double>>(3);
    auto maxAndArgMax = g.AddNode<ArgMaxNode<double>>(in->output);
    auto minAndArgMin = g.AddNode<ArgMinNode<double>>(in->output);
    auto condition = g.AddNode<ConstantNode<bool>>(true);
    auto valSelector = g.AddNode<ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, maxAndArgMax->val);
    auto indexSelector = g.AddNode<ValueSelectorNode<int>>(condition->output, maxAndArgMax->argVal, minAndArgMin->argVal);


    // 


    std::cout << "\nSelected value:" << std::endl;
    PrintGraph(g, valSelector);
    
    std::cout << "\nSelected index:" << std::endl;
    PrintGraph(g, indexSelector);

    std::cout << "\nfullGraph:" << std::endl;
    PrintGraph(g);

    // Set the input node's current values
    in->SetInput({0.25, 0.5, 0.75});

    std::cout << "\nComputing output of Input node" << std::endl;
    auto output1 = g.GetNodeOutput(in->output);
    for(auto x: output1) std::cout << x << "  ";
    std::cout << std::endl;

    // std::cout << "\nComputing output of ArgMax node" << std::endl;
    // auto output2 = g.GetNodeOutput(maxAndArgMax->val);
    // for(auto x: output2) std::cout << x << "  ";
    // std::cout << std::endl;

    std::cout << "\nComputing output of valSelector node" << std::endl;
    auto output3 = g.GetNodeOutput(valSelector->output);
    for(auto x: output3) std::cout << x << "  ";
    std::cout << std::endl;

    std::cout << "\nComputing output of indexSelector node" << std::endl;
    auto output4 = g.GetNodeOutput(indexSelector->output);
    for(auto x: output4) std::cout << x << "  ";
    std::cout << std::endl;
    
    // Now copy graph

    // g.Compute({ 1,2,3 });
    // auto output = valSelector.output.GetValue();
    // std::cout << "val: " << output << std::endl;
    
    //auto map = g.GetMap({ selector.output });
    //auto output2 = map.Compute({ 1,2,3 });
}