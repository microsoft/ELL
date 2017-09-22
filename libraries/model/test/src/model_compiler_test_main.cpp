////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     model_compiler_test_main.cpp (model_compiler_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableNodesTest.h"
#include "CompilerTest.h"
#include "ModelTestUtilities.h"
#include "PerformanceCountersTest.h"

// testing
#include "testing.h"

#include <iostream>

using namespace ell;
using namespace ell::emitters;
using namespace ell::predictors::neural;

void TestIRCompiler()
{
    // VerboseRegion region;
    // TestReceptiveFieldMatrixNode(2, false); // old (slow) version -- Fails
    // return;

    TestFloatNode();
    TestMultipleOutputNodes();
    TestCompilableDotProductNode2(3);
    TestCompilableDotProductNode2(4);

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
    TestSteppableMap(false);
    // TestSteppableMap(true); // Occassionally fails
    // TestMultiplexer(); // FAILS -- crash
    // TestLinearPredictor(); // FAILS -- crash
    // TestForest(); // FAILS -- crash

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
    TestCompilableTypeCastNode();
    TestReorderDataNode1();
    TestReorderDataNode2();
    TestReceptiveFieldMatrixNode(1, true); // new version
    TestReceptiveFieldMatrixNode(1, false); // old (slow) version
    TestReceptiveFieldMatrixNode(2, true); // new version
    // TestReceptiveFieldMatrixNode(2, false); // old (slow) version -- Fails
    TestCompilableAccumulatorNodeFunction();
    TestCompilableSourceNode(false);
    // TestCompilableSourceNode(true); // Occassionally fails
    TestCompilableSinkNode(false);
    TestCompilableSinkNode(true);
    TestCompilableAccumulatorNodeFunction();

    TestPerformanceCounters();
    TestCompilableDotProductNode2(3); // uses IR
    TestCompilableDotProductNode2(4); // uses IR

    //
    // Neural net nodes
    //
    TestNeuralNetworkPredictorNode1();
    TestNeuralNetworkPredictorNode2();
    TestNeuralNetworkPredictorNode3();
    // TestNeuralNetworkPredictorNode4(); // Currently fails
    // TestNeuralNetworkPredictorNode5(); // Currently fails (but just barely...)
    // TestNeuralNetworkPredictorNode6();

    // TestInputLayerNode(0);
    TestInputLayerNode(1);

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

    TestMaxPoolingLayerNode(8, 8, 6, 6, 3, 1, 0, 0); // params: inW, inH, outW, outH, poolingWindowSize, stride, inputPadding, outputPadding
    TestMaxPoolingLayerNode(8, 8, 6, 6, 3, 1, 0, 1);
    TestMaxPoolingLayerNode(8, 8, 6, 6, 3, 1, 0, 2);

    TestMaxPoolingLayerNode(10, 10, 5, 5, 3, 2, 1, 0);    
    TestMaxPoolingLayerNode(10, 10, 9, 9, 2, 1, 0, 0);

    // test weird case we are seeing in some cntk models
    TestMaxPoolingLayerNode(7, 7, 4, 4, 2, 2, 1, 0);

    TestMeanPoolingLayerNode(8, 8, 6, 6, 3, 1, 0, 0);
    TestMeanPoolingLayerNode(8, 8, 6, 6, 3, 1, 0, 1);
    TestMeanPoolingLayerNode(8, 8, 6, 6, 3, 1, 0, 2);
    // TestMeanPoolingLayerNode(8, 8, 6, 6, 3, 1, 1, 0);

    // TestMeanPoolingLayerNode(8, 8, 2, 1, 2, 1, 0, 0);

    TestScalingLayerNode();
    TestScalingLayerNode(0, 1);
    TestScalingLayerNode(0, 2);
    // TestScalingLayerNode(1, 0); // Input padding not supported (yet)

    TestSoftmaxLayerNode();
    TestSoftmaxLayerNode(0, 1);
    TestSoftmaxLayerNode(0, 2);
    // TestSoftmaxLayerNode(1, 0); // Input padding not supported (yet)

    TestBinaryConvolutionalLayerNode();
    TestBinaryConvolutionalLayerNode(1, 0, PaddingScheme::zeros, true);
    TestBinaryConvolutionalLayerNode(1, 0, PaddingScheme::minusOnes, false);
    TestBinaryConvolutionalLayerNode(1, 0, PaddingScheme::minusOnes, true);

    // TestConvolutionalLayerNode(ConvolutionType::GEMM);
    TestConvolutionalLayerNode(ConvolutionType::GEMM, 1, 0);
    TestConvolutionalLayerNode2(ConvolutionType::GEMM, 1, 0);
    // TestConvolutionalLayerNode(ConvolutionType::GEMM, 2, 0);
    // TestConvolutionalLayerNode(ConvolutionType::GEMM, 1, 1); // Convolutional layer output padding not supported

    TestConvolutionalLayerNode(ConvolutionType::Diagonal); // Input padding must be set correctly (to floor(filterWidth/2))

    TestFullyConnectedLayerNode();
    // TestFullyConnectedLayerNode(0, 1); // Fully-connected layer nodes can't have padding (yet)
    // TestFullyConnectedLayerNode(0, 2); // Fully-connected layer nodes can't have padding (yet)
    // TestFullyConnectedLayerNode(1, 1); // Fully-connected layer nodes can't have padding (yet)

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
        std::cerr << "ERROR, got ELL unhandled exception. Message: " << exception.what() << std::endl;
        return 1;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
