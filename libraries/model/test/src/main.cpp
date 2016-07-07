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
    TestExampleGraph();
    TestInputRouting1();
    TestInputRouting2();
    
    TestCopyGraph();
    TestRefineGraph();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}