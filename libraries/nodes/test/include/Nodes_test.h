/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Nodes_test.h (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

void TestL2NormNodeCompute();
void TestAccumulatorNodeCompute();
void TestDelayNodeCompute();
void TestMovingAverageNodeCompute();
void TestMovingVarianceNodeCompute();
void TestUnaryOperationNodeCompute();
void TestUnaryOperationNodeCompute1();
void TestBinaryOperationNodeCompute();
template <typename ElementType>
void TestLinearPredictorNodeCompute();
void TestDemultiplexerNodeCompute();
void TestDTWDistanceNodeCompute();
void TestSourceNodeCompute();
void TestSinkNodeCompute();
void TestEuclideanDistanceNodeCompute();

// Refinement
void TestMovingAverageNodeRefine();
template <typename ElementType>
void TestLinearPredictorNodeRefine();
void TestSimpleForestPredictorNodeRefine();
void TestDemultiplexerNodeRefine();
void TestMatrixVectorProductRefine();
void TestEuclideanDistanceNodeRefine();
void TestProtoNNPredictorNode();

#include "../tcc/Nodes_test.tcc"