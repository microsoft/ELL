////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Nodes_test.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>

using namespace ell;

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
        TestUnaryOperationNodeCompute1();
        TestBinaryOperationNodeCompute();
        TestLinearPredictorNodeCompute();
        TestDemultiplexerNodeCompute();
        TestDTWDistanceNodeCompute();
        TestSourceNodeCompute();
        TestSinkNodeCompute();

        TestBiasLayerNode();
        TestBatchNormalizationLayerNode();
        TestNeuralNetworkPredictorNode();

        TestMovingAverageNodeRefine();
        TestLinearPredictorNodeRefine();
        TestSimpleForestPredictorNodeRefine();
        TestDemultiplexerNodeRefine();
        TestMatrixVectorProductRefine();
        TestProtoNNPredictorNode();
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
