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
    TestStaticGraph();

    TestDynamicGraph();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}