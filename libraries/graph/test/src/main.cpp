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

int main(int argc, char** argv)
{
    // Create a graph

    DirectedGraph g;
    auto in = g.AddNode<InputNode<double>>(3);
    auto maxAndArgMax = g.AddNode<ArgMaxNode<double>>(in->output);
    auto minAndArgMin = g.AddNode<ArgMinNode<double>>(in->output);
    auto condition = g.AddNode<ConstantNode<bool>>(true);
    auto selector = g.AddNode<ValueSelectorNode<double>>(condition->output, maxAndArgMax->val, minAndArgMin->val);

    std::cout << "Visiting graph" << std::endl;
    auto visitor = [](const Node& node)
    {
        std::cout << "x" << std::endl;        
    };
    
    g.Visit(visitor, condition);


    //g.Compute({ 1,2,3 });
    //auto output = selector.output.GetValue();

    //auto map = g.GetMap({ selector.output });
    //auto output2 = map.Compute({ 1,2,3 });
}