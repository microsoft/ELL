////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodesTest.h (compile_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstring>

void TestCompileIsEqual();
void TestCompilableScalarOutputNode();
void TestCompilableVectorOutputNode();
void TestCompilableAccumulatorNode();
void TestCompilableConstantNode();
void TestCompilableDotProductNode();
void TestCompilableDelayNode();
void TestCompilableDTWDistanceNode();
void TestCompilableMulticlassDTW();
void TestCompilableScalarSumNode();
void TestCompilableSumNode();
void TestCompilableUnaryOperationNode();
void TestCompilableBinaryOperationNode();
void TestCompilableScalarBinaryPredicateNode();
void TestCompilableBinaryPredicateNode();
void TestCompilableMultiplexerNode();
void TestCompilableTypeCastNode();
void TestCompilableAccumulatorNodeFunction();
void TestCompilableSourceNode(bool runJit);
void TestCompilableSinkNode(bool runJit);
void TestCompilableDotProductNode2(int dimension);
void TestFloatNode();

void TestBiasLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestBatchNormalizationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestNeuralNetworkPredictorNode();
