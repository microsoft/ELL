////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     model_compiler_test_main.cpp (model_compiler_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableNodesTest.h"
#include "CompilerTest.h"
#include "ModelHeaderOutputTest.h"
#include "ModelTestUtilities.h"
#include "PerformanceCountersTest.h"

// testing
#include "testing.h"

// stl
#include <iostream>

using namespace ell;
using namespace ell::emitters;
using namespace ell::predictors::neural;

void TestIRCompiler()
{
    TestFloatNode();
    TestMultipleOutputNodes();
    TestCompilableDotProductNode2<float>(3);
    TestCompilableDotProductNode2<double>(3);
    TestCompilableDotProductNode2<float>(4);
    TestCompilableDotProductNode2<double>(4);

    TestCompileIsEqual();
    TestSimpleMap(false);
    TestSimpleMap(true);
    TestCompiledMapMove();
    TestBinaryScalar();
    TestBinaryVector(true);
    TestBinaryVector(false);
    TestBinaryVector(true, true);
    TestDotProduct();
    TestSum(false);
    TestSum(true);
    TestAccumulator(false);
    TestAccumulator(true);
    TestDelay();
    TestSqrt();
    TestBinaryPredicate(false);
    TestSlidingAverage();
    TestDotProductOutput();
    TestLinearPredictor<double>();
    TestLinearPredictor<float>();
    // TestMultiplexer(); // FAILS -- crash
    // TestForest(); // FAILS -- crash

    TestMatrixVectorMultiplyNode(10, 5, true);
    TestMatrixVectorMultiplyNode(10, 5, false);
    TestMatrixMatrixMultiplyNode(4, 5, 6, true);
    TestMatrixMatrixMultiplyNode(4, 5, 6, false);
    // TestMatrixMatrixMultiplyNode(15, 25600, 27, false); // Fails due to numerical  issues

    TestCompilableScalarOutputNode();
    TestCompilableVectorOutputNode();
    TestCompilableAccumulatorNode();
    TestCompilableConstantNode();
    TestCompilableDotProductNode();
    TestCompilableDelayNode();
    TestCompilableDTWDistanceNode();
    TestCompilableMulticlassDTW();
    TestCompilableScalarSumNode();
    TestCompilableSumNode();
    TestCompilableUnaryOperationNode();
    TestCompilableBinaryOperationNode();
    TestCompilableScalarBinaryPredicateNode();
    TestCompilableBinaryPredicateNode();
    TestCompilableMultiplexerNode();
    TestCompilableTypeCastNode(1);
    TestCompilableTypeCastNode(2);
    TestCompilableTypeCastNode(10);
    TestReorderDataNode1();
    TestReorderDataNode2();
    TestReceptiveFieldMatrixNode(1, true); // new version
    TestReceptiveFieldMatrixNode(1, false); // old (slow) version
    TestReceptiveFieldMatrixNode(2, true); // new version
    // TestReceptiveFieldMatrixNode(2, false); // old (slow) version -- Fails
    TestCompilableAccumulatorNodeFunction();
    TestCompilableSourceNode();
    TestCompilableSinkNode();
    TestCompilableClockNode();
    TestCompilableFFTNode();

    TestPerformanceCounters();
    TestCompilableDotProductNode2<float>(3); // uses IR
    TestCompilableDotProductNode2<double>(3); // uses IR
    TestCompilableDotProductNode2<float>(4); // uses IR
    TestCompilableDotProductNode2<double>(4); // uses IR

    //
    // Neural net nodes
    //
    TestNeuralNetworkPredictorNode1();
    TestNeuralNetworkPredictorNode2();
    TestNeuralNetworkPredictorNode3();
    // TestNeuralNetworkPredictorNode4(); // Currently fails
    // TestNeuralNetworkPredictorNode5(); // Currently fails (but just barely...)
    // TestNeuralNetworkPredictorNode6();

    TestFusedLinearLayerNodes(4, 6, 8);

    // TestInputLayerNode(0);
    TestInputLayerNode(1);

    TestHardSigmoidActivationLayerNode();
    TestHardSigmoidActivationLayerNode(0, 1);
    TestHardSigmoidActivationLayerNode(0, 2);

    TestReLUActivationLayerNode();
    TestReLUActivationLayerNode(0, 1);
    TestReLUActivationLayerNode(0, 2);
    // TestReLUActivationLayerNode(1, 0); // Input padding not supported (yet)

    TestLeakyReLUActivationLayerNode();
    TestLeakyReLUActivationLayerNode(0, 1);
    TestLeakyReLUActivationLayerNode(0, 2);

    TestParametricReLUActivationLayerNode();
    TestParametricReLUActivationLayerNode(0, 1);
    TestParametricReLUActivationLayerNode(0, 2);

    TestSigmoidActivationLayerNode();
    TestSigmoidActivationLayerNode(0, 1);
    TestSigmoidActivationLayerNode(0, 2);

    TestBatchNormalizationLayerNode();
    TestBatchNormalizationLayerNode(0, 1);
    TestBatchNormalizationLayerNode(0, 2);
    // TestBatchNormalizationLayerNode(1, 0); // Input padding not supported (yet)

    TestBiasLayerNode();
    TestBiasLayerNode(0, 1);
    TestBiasLayerNode(0, 2);
    // TestBiasLayerNode(1, 0); // Input padding not supported (yet)

    TestMaxPoolingLayerNode(5, 5, 16, 5, 5, 4, 1, 1, 0); // params: inW, inH, inChannels, outW, outH, poolingWindowSize, stride, inputPadding, outputPadding

    TestMaxPoolingLayerNode(8, 8, 16, 6, 6, 3, 1, 0, 0); // params: inW, inH, inChannels, outW, outH, poolingWindowSize, stride, inputPadding, outputPadding
    TestMaxPoolingLayerNode(8, 8, 16, 6, 6, 3, 1, 0, 1);
    TestMaxPoolingLayerNode(8, 8, 16, 6, 6, 3, 1, 0, 2);

    TestMaxPoolingLayerNode(10, 10, 16, 5, 5, 3, 2, 1, 0);
    TestMaxPoolingLayerNode(10, 10, 16, 9, 9, 2, 1, 0, 0);

    // test weird case we are seeing in some cntk models
    TestMaxPoolingLayerNode(7, 7, 16, 4, 4, 2, 2, 0, 0);

    TestMeanPoolingLayerNode(8, 8, 16, 6, 6, 3, 1, 0, 0);
    TestMeanPoolingLayerNode(8, 8, 16, 6, 6, 3, 1, 0, 1);
    TestMeanPoolingLayerNode(8, 8, 16, 6, 6, 3, 1, 0, 2);
    // TestMeanPoolingLayerNode(8, 8, 16, 6, 6, 3, 1, 1, 0);

    // TestMeanPoolingLayerNode(8, 8, 16, 2, 1, 2, 1, 0, 0);

    TestScalingLayerNode();
    TestScalingLayerNode(0, 1);
    TestScalingLayerNode(0, 2);
    // TestScalingLayerNode(1, 0); // Input padding not supported (yet)

    TestSoftmaxLayerNode();
    TestSoftmaxLayerNode(0, 1);
    TestSoftmaxLayerNode(0, 2);
    // TestSoftmaxLayerNode(1, 0); // Input padding not supported (yet)

    TestBinaryConvolutionalLayerNode(32, 32, 3, 4);
    TestBinaryConvolutionalLayerNode(32, 32, 3, 4, 1, 0, PaddingScheme::zeros, true);
    TestBinaryConvolutionalLayerNode(32, 32, 3, 4, 1, 0, PaddingScheme::minusOnes, false);
    TestBinaryConvolutionalLayerNode(32, 32, 3, 4, 1, 0, PaddingScheme::minusOnes, true);

    // TestConvolutionalLayerNode(ConvolutionType::unrolled);
    TestConvolutionalLayerNode(ConvolutionType::unrolled, 1, 0);
    TestConvolutionalLayerNode2(ConvolutionType::unrolled, 1, 0);
    // TestConvolutionalLayerNode(ConvolutionType::unrolled, 2, 0);
    // TestConvolutionalLayerNode(ConvolutionType::unrolled, 1, 1); // Convolutional layer output padding not supported

    TestConvolutionalLayerNode(ConvolutionType::diagonal); // Input padding must be set correctly (to floor(filterWidth/2))
    
    TestConvolutionalLayerNode(ConvolutionType::simple); // Input padding must be set correctly (to floor(filterWidth/2))

    TestFullyConnectedLayerNode();
    // TestFullyConnectedLayerNode(0, 1); // Fully-connected layer nodes can't have padding (yet)
    // TestFullyConnectedLayerNode(0, 2); // Fully-connected layer nodes can't have padding (yet)
    // TestFullyConnectedLayerNode(1, 1); // Fully-connected layer nodes can't have padding (yet)

    TestProtoNNPredictorMap();
    TestMultiSourceSinkMap();

    TestRecurrentNode();
    TestGRUNode();
    TestLSTMNode();

    TestRegionDetectionNode();

    TestMatrixVectorProductNodeCompile();

    // Header file generation
    TestModelHeaderOutput();
}

int main(int argc, char* argv[])
{
    try
    {
        TestIRCompiler();
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ERROR, got unhandled exception. Message: " << exception.what() << std::endl;
        return 1;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
