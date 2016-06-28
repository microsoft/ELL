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
    TestInputRouting1();
    TestInputRouting2();
    TestStaticGraph();

    TestCopyGraph();
    TestRefineGraph();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}