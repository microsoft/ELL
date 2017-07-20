////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodesTest.h (compile_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstring> // size_t

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

//
// NN layer nodes
//
void TestNeuralNetworkPredictorNode1();
void TestNeuralNetworkPredictorNode2();
void TestNeuralNetworkPredictorNode3();
void TestNeuralNetworkPredictorNode4();

enum class ConvolutionType { GEMM, Diagonal};

void TestInputLayerNode(size_t outputPadding = 0);
void TestReLUActivationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestLeakyReLUActivationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestSigmoidActivationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestBatchNormalizationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestBiasLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestBinaryConvolutionalLayerNode(size_t inputPadding = 1, size_t outputPadding = 0);
void TestBinaryConvolutionalLayerNode2(size_t inputPadding = 1, size_t outputPadding = 0);
void TestConvolutionalLayerNode(ConvolutionType convolutionType, size_t inputPadding = 1, size_t outputPadding = 0);
void TestConvolutionalLayerNode2(ConvolutionType convolutionType, size_t inputPadding = 1, size_t outputPadding = 0);
void TestFullyConnectedLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestMaxPoolingLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestMeanPoolingLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestScalingLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestSoftmaxLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
