////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (predictors_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestPredictorTests.h"
#include "NeuralNetworkPredictorTests.h"
#include "ProtoNNPredictorTests.h"

// testing
#include "testing.h"

/// Runs all tests
///
int main()
{
    // ForestPredictor
    ForestPredictorTest();

    // NeuralNetworkPredictor
    ActivationTest<float>();
    LayerBaseTest<float>();
    ActivationLayerTest<float>();
    BatchNormalizationLayerTest<float>();
    BiasLayerTest<float>();
    InputLayerTest<float>();
    ScalingLayerTest<float>();
    FullyConnectedLayerTest<float>();
    PoolingLayerTest<float>();
    ConvolutionalLayerTest<float>();
    BinaryConvolutionalLayerBitwiseTest<float>();
    BinaryConvolutionalLayerGemmTest<float>();
    SoftmaxLayerTest<float>();
    NeuralNetworkPredictorTest<float>();

    ActivationTest<double>();
    LayerBaseTest<double>();
    ActivationLayerTest<double>();
    BatchNormalizationLayerTest<double>();
    BiasLayerTest<double>();
    InputLayerTest<double>();
    ScalingLayerTest<double>();
    FullyConnectedLayerTest<double>();
    PoolingLayerTest<double>();
    ConvolutionalLayerTest<double>();
    BinaryConvolutionalLayerBitwiseTest<double>();
    BinaryConvolutionalLayerGemmTest<double>();
    SoftmaxLayerTest<double>();
    NeuralNetworkPredictorTest<double>();
    ConvolutionalArchiveTest<float>();
    BinaryConvolutionalArchiveTest<float>();

    ProtoNNPredictorTest();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}