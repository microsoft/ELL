////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (passes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Optimization pass tests
//

#include "ModelOptimizerTest.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>

using namespace ell;

int main()
{
    try
    {
        TestModelOptimizer();
        TestModelCompilePlusOptimize();
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ERROR, got unhandled exception. Message: " << exception.what() << std::endl;
        return 1;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
