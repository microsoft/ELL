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
    TestL2NormNodeCompute();
    TestAccumulatorNodeCompute();
    TestDelayNodeCompute();
    TestMovingAverageNodeCompute();
    TestMovingVarianceNodeCompute(); 
    TestUnaryOperationNodeCompute();
    TestBinaryOperationNodeCompute();
    TestLinearPredictorNodeCompute();
    
    TestMovingAverageNodeRefine();
    TestLinearPredictorNodeRefine();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
