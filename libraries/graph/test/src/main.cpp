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



int main()
{
    TestStaticGraph();

    TestDynamicGraph();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}