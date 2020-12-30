/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BasicMathNodesTests.h (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Main driver function
void TestBasicMathNodes();

// Sub-tests called by main driver
void TestBinaryOperationNodeCompute();
void TestBinaryOperationNodeCompute2();
void TestUnaryOperationNodeCompute();
void TestLogicalUnaryOperationNodeCompute();
void TestBroadcastLinearFunctionNodeCompute();

void TestBroadcastUnaryOperationNodeCompute();
void TestBroadcastBinaryOperationNodeComputeFull();
void TestBroadcastBinaryOperationNodeComputeAdd();
void TestBroadcastBinaryOperationNodeComputeSubtract();
void TestBroadcastBinaryOperationNodeComputeWithOrdering();
void TestBroadcastBinaryOperationNodeComputeWithLayout();
void TestBroadcastBinaryOperationNodeComputeWithBadLayout();
void TestBroadcastBinaryOperationNodeComputeDifferentBroadcastDimensions();
void TestBroadcastTernaryOperationNodeComputeFMA();
