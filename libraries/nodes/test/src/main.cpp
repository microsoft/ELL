////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NeuralNetworkLayerNodesTest.h"
#include "Nodes_test.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"
#include "Unused.h"

// stl
#include <iostream>

using namespace ell;

/// Runs all tests
///
int main(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    try
    {
        NodesTests();
        NeuralNetworkLayerNodesTests();
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        throw;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
