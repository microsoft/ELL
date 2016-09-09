////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nodes_test.h"

// utilities
#include "Exception.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>

using namespace emll;

/// Runs all tests
///
int main()
{
    try
    {
        TestL2NormNodeCompute();
        TestAccumulatorNodeCompute();
        TestDelayNodeCompute();
        TestMovingAverageNodeCompute();
        TestMovingVarianceNodeCompute();
        TestUnaryOperationNodeCompute();
        TestBinaryOperationNodeCompute();
        TestLinearPredictorNodeCompute();
        TestDemultiplexerNodeCompute();

        TestMovingAverageNodeRefine();
        TestLinearPredictorNodeRefine();
        TestSimpleForestPredictorNodeRefine();
        TestDemultiplexerNodeRefine();
    }
    catch (const utilities::Exception& exception)
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
