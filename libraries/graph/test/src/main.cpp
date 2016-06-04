//
// Graph tests
// 

#include "DirectedGraph.h"
#include "ValueSelectorNode.h"
#include "ExtremalValNode.h"
#include "ConstantNode.h"
#include "InputNode.h"
#include "NodeInput.h"
#include "NodeOutput.h"

#include <iostream>

// template <typename... Args>
// auto concat(Args... args)
// {
//     return { args... };
// }

void PrintGraph(const DirectedGraph& graph, const std::shared_ptr<Node>& output)
{
    auto visitor = [](const Node& node)
    {
        std::cout << node.Type() << std::endl;        
    };
    
    graph.Visit(visitor, output);    
}

void CopyGraph(const DirectedGraph& graph1, const std::shared_ptr<Node>& output,  DirectedGraph& graph2)
{
    auto visitor = [](const Node& node)
    {
        std::cout << node.Type() << std::endl;        
    };
    
    graph1.Visit(visitor, output);
}

int main(int argc, char** argv)
{
    // Create a graph

    DirectedGraph g;
    auto in = g.AddNode<InputNode<double>>(3);
    auto maxAndArgMax = g.AddNode<ArgMaxNode<double>>(in->_output); // segfault
    auto minAndArgMin = g.AddNode<ArgMinNode<double>>(in->_output);
    auto condition = g.AddNode<ConstantNode<bool>>(true);
    auto selector = g.AddNode<ValueSelectorNode<double>>(condition->_output, maxAndArgMax->_val, minAndArgMin->_val);

    PrintGraph(g, selector);
    
    // Now copy graph

    //g.Compute({ 1,2,3 });
    //auto output = selector.output.GetValue();

    //auto map = g.GetMap({ selector.output });
    //auto output2 = map.Compute({ 1,2,3 });
}