////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (evaluators_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// Runs all tests
///

#include "Evaluators_test.h"

// evaluators
#include "Evaluator.h"

// testing
#include "testing.h"

// stl
#include <iostream>

using namespace ell;

int main()
{
    try
    {
        TestEvaluators();
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
