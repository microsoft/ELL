//
// Graph tests
// 

#include "DirectedGraph.h"
#include "ValueSelectorNode.h"
#include "ArgMaxNode.h"
#include "ConstantNode.h"
#include "InputNode.h"
#include "NodeInput.h"
#include "NodeOutput.h"


template <typename... Args>
auto concat(Args... args)
{
    return { args... };
}

int main(int argc, char** argv)
{
    // Create a grpah

    DirectedGraph g;
    auto in = g.AddNode<InputNode<double>>(3);
    auto maxAndArgMax = g.AddNode<ArgMaxNode<double>>(in->output);
    auto minAndArgMin = g.AddNode<ArgMinNode<double>>(in->output);
    auto condition = g.AddNode<ConstantNode<bool>>(true);

    auto selector = g.AddNode<ValueSelectorNode<double>>(condition->output, concat(maxAndArgMax->extremalVal, minAndArgMin->extremalVal));

    //g.Compute({ 1,2,3 });
    //auto output = selector.output.GetValue();

    //auto map = g.GetMap({ selector.output });
    //auto output2 = map.Compute({ 1,2,3 });
}