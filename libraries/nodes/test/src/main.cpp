////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nodes_test.h"

// testing
#include "testing.h"

/// Runs all tests
///
int main()
{
    TestMagnitudeNodeCompute();
    TestAccumulatorNodeCompute();
    TestDelayNodeCompute();
    TestMovingAverageNodeCompute();
    TestMovingVarianceNodeCompute(); 
    TestUnaryOperationNodeCompute();
    TestBinaryOperationNodeCompute();
    TestLinearPredictorNodeCompute();
    
    TestMovingAverageNodeRefine();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
