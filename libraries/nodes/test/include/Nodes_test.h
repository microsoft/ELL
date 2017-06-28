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
void TestLinearPredictorNodeCompute();
void TestDemultiplexerNodeCompute();
void TestDTWDistanceNodeCompute();
void TestSourceNodeCompute();
void TestSinkNodeCompute();

// Refinement
void TestMovingAverageNodeRefine();
void TestLinearPredictorNodeRefine();
void TestSimpleForestPredictorNodeRefine();
void TestDemultiplexerNodeRefine();
void TestMatrixVectorProductRefine();
void TestProtoNNPredictorNode();
