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

// stl
#include <iostream>

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
        TestMultiplexorNodeCompute();

        TestMovingAverageNodeRefine();
        TestLinearPredictorNodeRefine();
        TestSimpleForestNodeRefine();
    }
    catch(utilities::Exception& exception)
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
