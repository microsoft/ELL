//
// Graph tests
// 

#include "Graph_test.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <unordered_map>



int main()
{
    TestMultifariousInputs();
    TestStaticGraph();
    TestDynamicGraph();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}