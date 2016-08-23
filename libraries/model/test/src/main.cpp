//
// Model tests
// 

#include "Graph_test.h"
#include "PortElements_test.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>

int main()
{
    try
    {
        TestStaticGraph();
        TestNodeIterator();
        TestExampleGraph();
        TestInputRouting1();
        TestInputRouting2();

        TestCopyGraph();
        TestRefineGraph();
        TestRefineSplitOutputs();

        // PortElements tests
        TestSlice();
        TestAppend();
    }
    catch(const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got EMLL exception. Message: " << exception.GetMessage() << std::endl;
        throw;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}