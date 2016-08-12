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
    TestSimpleForestNodeRefine();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
