////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodesTest.h (compile_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// predictors
#include <Layer.h>

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
void TestCompilableUnaryOperation_square_Node();
void TestL2NormSquaredNodeCompiled();
void TestMatrixVectorProductNodeCompile();
void TestCompilableBinaryOperationNode();
void TestCompilableScalarBinaryPredicateNode();
void TestCompilableBinaryPredicateNode();
void TestCompilableMultiplexerNode();
void TestCompilableTypeCastNode(size_t dimension);
void TestReorderDataNode1();
void TestReorderDataNode2();
void TestReceptiveFieldMatrixNode(size_t numChannels, bool useNewReshape);
void TestCompilableAccumulatorNodeFunction();
void TestCompilableSourceNode();
void TestCompilableSinkNode();
template <typename ElementType>
void TestCompilableDotProductNode2(int dimension);
void TestFloatNode();
void TestMultipleOutputNodes();
void TestCompilableClockNode();
void TestCompilableFFTNode();

//
// mathy nodes
//
void TestMatrixVectorMultiplyNode(int m, int n, bool useBlas);
void TestMatrixMatrixMultiplyNode(int m, int n, int k, bool useBlas);

//
// NN layer nodes
//
void TestNeuralNetworkPredictorNode1();
void TestNeuralNetworkPredictorNode2();
void TestNeuralNetworkPredictorNode3();
void TestNeuralNetworkPredictorNode4();
void TestNeuralNetworkPredictorNode5();
void TestNeuralNetworkPredictorNode6();

enum class ConvolutionType
{
    simple,
    unrolled,
    diagonal
};

void TestInputLayerNode(size_t outputPadding = 0);
void TestHardSigmoidActivationLayerNode(size_t inputPaddingSize = 0, size_t outputPaddingSize = 0);
void TestReLUActivationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestLeakyReLUActivationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestParametricReLUActivationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestSigmoidActivationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestBatchNormalizationLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestBiasLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestBinaryConvolutionalLayerNode(size_t imageRows, size_t imageColumns, size_t numChannels, size_t numFilters, size_t inputPadding = 1, size_t outputPadding = 0, ell::predictors::neural::PaddingScheme = ell::predictors::neural::PaddingScheme::zeros, bool scaleByFilterMeans = true);
void TestConvolutionalLayerNode(ConvolutionType convolutionType, size_t inputPadding = 1, size_t outputPadding = 0);
void TestConvolutionalLayerNode2(ConvolutionType convolutionType, size_t inputPadding = 1, size_t outputPadding = 0);
void TestFullyConnectedLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestMaxPoolingLayerNode(size_t inRows, size_t inCols, size_t numChannels, size_t outRows, size_t outCols, size_t poolingSize, size_t poolingStride, size_t inputPadding = 0, size_t outputPadding = 0);
void TestMeanPoolingLayerNode(size_t inRows, size_t inCols, size_t numChannels, size_t outRows, size_t outCols, size_t poolingSize, size_t poolingStride, size_t inputPadding = 0, size_t outputPadding = 0);
void TestScalingLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestSoftmaxLayerNode(size_t inputPadding = 0, size_t outputPadding = 0);
void TestFusedLinearLayerNodes(size_t rows, size_t columns, size_t channels);
void TestRecurrentNode();
void TestGRUNode();
void TestLSTMNode();
void TestRegionDetectionNode();

#include "../tcc/CompilableNodesTest.tcc"
