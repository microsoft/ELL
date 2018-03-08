////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorTests.tcc (predictors_test)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GRULayer.h"
#include "HardSigmoidActivation.h"
#include "LeakyReLUActivation.h"
#include "LSTMLayer.h"
#include "MaxPoolingFunction.h"
#include "NeuralNetworkPredictor.h"
#include "ParametricReLUActivation.h"
#include "ReLUActivation.h"
#include "RecurrentLayer.h"
#include "SigmoidActivation.h"
#include "SoftMaxActivation.h"
#include "TanhActivation.h"

// testing
#include "testing.h"

// utilities
#include "JsonArchiver.h"

using namespace ell;

inline bool Equals(double a, double b)
{
    if (std::abs(a - b) < 0.0001)
        return true;
    return false;
}

template <typename ElementType>
void ActivationTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using TensorType = typename Layer<ElementType>::TensorType;

    TensorType T0(2, 2, 2);
    T0(0, 0, 0) = static_cast<ElementType>(1.0);
    T0(0, 1, 0) = static_cast<ElementType>(-2.0);
    T0(1, 0, 1) = static_cast<ElementType>(3.0);
    T0(1, 1, 1) = static_cast<ElementType>(-4.0);

    TensorType T1(2, 2, 2);

    auto hardSigmoid = HardSigmoidActivation<ElementType>();
    for (size_t i = 0; i < T0.NumRows(); ++i)
    {
        for (size_t j = 0; j < T0.NumColumns(); ++j)
        {
            for (size_t k = 0; k < T0.NumChannels(); ++k)
            {
                T1(i, j, k) = hardSigmoid.Apply(T0(i, j, k), { i, j, k });
            }
        }
    }
    testing::ProcessTest("Testing HardSigmoidActivation", Equals(T1(0, 0, 0), 0.7) && Equals(T1(0, 1, 0), 0.1) && T1(1, 0, 1) == 1 && T1(1, 1, 1) == 0 &&
                                                          T1(0, 0, 1) == 0.5 && T1(0, 1, 1) == 0.5 && T1(1, 0, 0) == 0.5 && T1(1, 1, 0) == 0.5);

    auto relu = ReLUActivation<ElementType>();
    for (size_t i = 0; i < T0.NumRows(); ++i)
    {
        for (size_t j = 0; j < T0.NumColumns(); ++j)
        {
            for (size_t k = 0; k < T0.NumChannels(); ++k)
            {
                T1(i, j, k) = relu.Apply(T0(i, j, k), { i, j, k });
            }
        }
    }
    testing::ProcessTest("Testing ReLUActivation", T1(0, 0, 0) == 1.0 && T1(0, 1, 0) == 0 && T1(1, 0, 1) == 3.0 && T1(1, 1, 1) == 0);

    auto leakyRelu = LeakyReLUActivation<ElementType>(static_cast<ElementType>(0.1));
    for (size_t i = 0; i < T0.NumRows(); ++i)
    {
        for (size_t j = 0; j < T0.NumColumns(); ++j)
        {
            for (size_t k = 0; k < T0.NumChannels(); ++k)
            {
                T1(i, j, k) = leakyRelu.Apply(T0(i, j, k), { i, j, k });
            }
        }
    }
    testing::ProcessTest("Testing LeakyReLUActivation", Equals(T1(0, 0, 0), 1.0) && Equals(T1(0, 1, 0), -0.2) && Equals(T1(1, 0, 1), 3.0) && Equals(T1(1, 1, 1), -0.4));

    TensorType alpha(2, 2, 2);
    alpha(0, 0, 0) = static_cast<ElementType>(0.1);
    alpha(0, 1, 0) = static_cast<ElementType>(0.2);
    alpha(1, 0, 1) = static_cast<ElementType>(0.3);
    alpha(1, 1, 1) = static_cast<ElementType>(0.4);

    auto parametricRelu = ParametricReLUActivation<ElementType>(alpha);
    for (size_t i = 0; i < T0.NumRows(); ++i)
    {
        for (size_t j = 0; j < T0.NumColumns(); ++j)
        {
            for (size_t k = 0; k < T0.NumChannels(); ++k)
            {
                T1(i, j, k) = parametricRelu.Apply(T0(i, j, k), { i, j, k });
            }
        }
    }
    testing::ProcessTest("Testing ParametricReLUActivation", Equals(T1(0, 0, 0), 1.0) && Equals(T1(0, 1, 0), -0.4) && Equals(T1(1, 0, 1), 3.0) && Equals(T1(1, 1, 1), -1.6));

    auto sigmoid = SigmoidActivation<ElementType>();
    for (size_t i = 0; i < T0.NumRows(); ++i)
    {
        for (size_t j = 0; j < T0.NumColumns(); ++j)
        {
            for (size_t k = 0; k < T0.NumChannels(); ++k)
            {
                T1(i, j, k) = sigmoid.Apply(T0(i, j, k), { i, j, k });
            }
        }
    }
    testing::ProcessTest("Testing SigmoidActivation", Equals(T1(0, 0, 0), 0.73106) && Equals(T1(0, 1, 0), 0.11920) && Equals(T1(1, 0, 1), 0.95257) && Equals(T1(1, 1, 1), 0.017986));
}

template <typename ElementType>
void LayerBaseTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    // Verify LayerBase
    TensorType input0(12, 12, 3);
    PaddingParameters paddingParameters2{ PaddingScheme::alternatingZeroAndOnes, 1 };
    Shape outputShape = { 12, 12, 6 };
    LayerParameters layerParameters{ input0, ZeroPadding(1), outputShape, paddingParameters2 };

    Layer<ElementType> baseLayer(layerParameters);
    auto layerBaseOutput = baseLayer.GetOutput();
    testing::ProcessTest("Testing LayerBase, output tensor", layerBaseOutput.NumRows() == 12 && layerBaseOutput.NumColumns() == 12 && layerBaseOutput.NumChannels() == 6);
    testing::ProcessTest("Testing LayerBase, output tensor padding values", layerBaseOutput(0, 0, 0) == 0 && layerBaseOutput(0, 1, 0) == 1 && layerBaseOutput(0, 2, 0) == 0 && layerBaseOutput(0, 3, 0) == 1);
}

template <typename ElementType>
void ActivationLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    // Verify ActivationLayer
    TensorType activationInput(2, 2, 2);
    activationInput(0, 0, 0) = 1.0;
    activationInput(0, 1, 0) = -2.0;
    activationInput(1, 0, 1) = 3.0;
    activationInput(1, 1, 1) = -4.0;
    Shape activationOutputShape = { 4, 4, 2 };
    LayerParameters activationParameters{ activationInput, NoPadding(), activationOutputShape, ZeroPadding(1) };

    ActivationLayer<ElementType, ReLUActivation> activationLayer(activationParameters);
    activationLayer.Compute();
    auto output0 = activationLayer.GetOutput();
    testing::ProcessTest("Testing ActivationLayer, values", output0(1, 1, 0) == 1.0 && output0(1, 2, 0) == 0 && output0(2, 1, 1) == 3.0 && output0(2, 2, 1) == 0);
    testing::ProcessTest("Testing ActivationLayer, padding", output0(0, 0, 0) == 0 && output0(0, 1, 0) == 0 && output0(2, 3, 1) == 0 && output0(3, 3, 1) == 0);
}

template <typename ElementType>
void BatchNormalizationLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    // Verify BatchNormailzationLayer
    TensorType bnInput(2, 2, 2);
    bnInput(0, 0, 0) = 11;
    bnInput(0, 1, 0) = 7;
    bnInput(1, 0, 1) = 30;
    bnInput(1, 1, 1) = 50;
    Shape bnOutputShape = { 4, 4, 2 };
    LayerParameters bnParameters{ bnInput, NoPadding(), bnOutputShape, ZeroPadding(1) };
    VectorType mean({ 5, 10 });
    VectorType variance({ 4.0, 16.0 });

    BatchNormalizationLayer<ElementType> bnLayer(bnParameters, mean, variance, static_cast<ElementType>(1e-6), EpsilonSummand::SqrtVariance);
    bnLayer.Compute();
    auto output1 = bnLayer.GetOutput();
    testing::ProcessTest("Testing BatchNormailzationLayer, values", Equals(output1(1, 1, 0), 3.0) && Equals(output1(1, 2, 0), 1.0) && Equals(output1(2, 1, 1), 5.0) && Equals(output1(2, 2, 1), 10.0));
    testing::ProcessTest("Testing BatchNormailzationLayer, padding", output1(0, 0, 0) == 0 && output1(0, 1, 0) == 0 && output1(2, 3, 1) == 0 && output1(3, 3, 1) == 0);
}

template <typename ElementType>
void BiasLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    // Verify BiasLayer
    TensorType input(2, 2, 2);
    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;
    Shape outputShape = { 4, 4, 2 };
    LayerParameters parameters{ input, NoPadding(), outputShape, ZeroPadding(1) };
    VectorType bias({ 5, 10 });

    BiasLayer<ElementType> biasLayer(parameters, bias);
    biasLayer.Compute();
    auto output = biasLayer.GetOutput();
    testing::ProcessTest("Testing BiasLayer, values", Equals(output(1, 1, 0), 6.0) && Equals(output(1, 2, 0), 7.0) && Equals(output(2, 1, 1), 13.0) && Equals(output(2, 2, 1), 14.0));
    testing::ProcessTest("Testing BiasLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);
}

template <typename ElementType>
void InputLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using Shape = typename Layer<ElementType>::Shape;

    // Verify Input
    Shape inputShape = { 2, 2, 2 };
    Shape outputShape = { 4, 4, 2 };
    typename InputLayer<ElementType>::InputParameters parameters{ inputShape, NoPadding(), outputShape, ZeroPadding(1), 2.0 };

    InputLayer<ElementType> inputLayer(parameters);
    inputLayer.SetInput(std::vector<ElementType>({ 1, 2, 3, 4, 5, 6, 7, 8 }));
    inputLayer.Compute();
    auto output = inputLayer.GetOutput();
    testing::ProcessTest("Testing InputLayer, values", Equals(output(1, 1, 0), 2.0) && Equals(output(1, 2, 0), 6.0) && Equals(output(2, 1, 1), 12.0) && Equals(output(2, 2, 1), 16.0));
    testing::ProcessTest("Testing InputLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);
}

template <typename ElementType>
void ScalingLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    // Verify BiasLayer
    TensorType input(2, 2, 2);
    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;
    Shape outputShape = { 4, 4, 2 };
    LayerParameters parameters{ input, NoPadding(), outputShape, ZeroPadding(1) };
    VectorType scales({ 2, 0.5 });

    ScalingLayer<ElementType> scalingLayer(parameters, scales);
    scalingLayer.Compute();
    auto output = scalingLayer.GetOutput();
    testing::ProcessTest("Testing ScalingLayer, values", Equals(output(1, 1, 0), 2.0) && Equals(output(1, 2, 0), 4) && Equals(output(2, 1, 1), 1.5) && Equals(output(2, 2, 1), 2.0));
    testing::ProcessTest("Testing ScalingLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);
}

template <typename ElementType>
void FullyConnectedLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    // Verify FullyConnectedLayer
    TensorType input(2, 2, 1);
    input.Fill(1);
    Shape outputShape = { 3, 5, 1 };
    LayerParameters parameters{ input, NoPadding(), outputShape, ZeroPadding(1) };
    MatrixType weights(3, 4);
    weights(0, 0) = 1;
    weights(0, 1) = 1;
    weights(0, 2) = 1;
    weights(0, 3) = 2;
    weights(1, 0) = 1;
    weights(1, 1) = 1;
    weights(1, 2) = 1;
    weights(1, 3) = 3;
    weights(2, 0) = 1;
    weights(2, 1) = 1;
    weights(2, 2) = 1;
    weights(2, 3) = 4;

    FullyConnectedLayer<ElementType> connectedLayer(parameters, weights);
    connectedLayer.Compute();
    auto output = connectedLayer.GetOutput();
    testing::ProcessTest("Testing FullyConnectedLayer, values", Equals(output(1, 1, 0), 5.0) && Equals(output(1, 2, 0), 6.0) && Equals(output(1, 3, 0), 7.0));
    testing::ProcessTest("Testing FullyConnectedLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(1, 4, 0) == 0 && output(2, 4, 0) == 0);
}

template <typename ElementType>
void PoolingLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    // Verify PoolingLayer with no padding
    TensorType input(4, 4, 2);
    input.Fill(1);
    input(1, 1, 0) = 10;
    input(0, 2, 0) = 20;
    input(2, 0, 0) = 30;
    input(3, 3, 0) = 40;
    input(1, 1, 1) = 11;
    input(0, 2, 1) = 21;
    input(2, 0, 1) = 31;
    input(3, 3, 1) = 41;
    Shape outputShape = { 4, 4, 2 };
    LayerParameters parameters{ input, NoPadding(), outputShape, ZeroPadding(1) };
    PoolingParameters poolingParams{ 2, 2 };
    PoolingLayer<ElementType, MaxPoolingFunction> poolingLayer(parameters, poolingParams);
    poolingLayer.Compute();
    auto output = poolingLayer.GetOutput();

    testing::ProcessTest("Testing PoolingLayer, values", Equals(output(1, 1, 0), 10) && Equals(output(1, 2, 0), 20) && Equals(output(2, 1, 0), 30) && Equals(output(2, 2, 0), 40) && Equals(output(1, 1, 1), 11) && Equals(output(1, 2, 1), 21) && Equals(output(2, 1, 1), 31) && Equals(output(2, 2, 1), 41));
    testing::ProcessTest("Testing PoolingLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);

    // Verify PoolingLayer with padding
    TensorType input2 // This input must include the padding
        {
          { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
          { { 0, -1 }, { 5, 6 }, { 0, 0 }, { 20, 21 }, { 0, 0 }, { 0, 0 } },
          { { 0, 0 }, { -1, 0 }, { 10, 11 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
          { { 0, 0 }, { 30, 31 }, { 0, 0 }, { 0, 0 }, { -1, 0 }, { 0, 0 } },
          { { 0, 0 }, { 0, 0 }, { 0, -5 }, { 0, 0 }, { 40, 41 }, { 0, 0 } },
          { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, -1 }, { 0, 0 }, { 0, 0 } },
        };
    TensorType expected2{
        { { 5, 6 }, { 20, 21 }, { 0, 0 } },
        { { 30, 31 }, { 10, 11 }, { 0, 0 } },
        { { 0, 0 }, { 0, 0 }, { 40, 41 } },
    };

    Shape outputShape2 = { 3, 3, 2 };
    LayerParameters parameters2{ input2, ZeroPadding(1), outputShape2, NoPadding() };
    PoolingParameters poolingParams2{ 2, 2 };
    PoolingLayer<ElementType, MaxPoolingFunction> poolingLayer2(parameters2, poolingParams2);
    poolingLayer2.Compute();
    auto output2 = poolingLayer2.GetOutput();

    testing::ProcessTest("Testing PoolingLayer with padding, values", output2.IsEqual(expected2));
}

template <typename ElementType>
void ConvolutionalLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    // Verify ConvolutionalLayer with diagonal method
    TensorType input(3, 4, 2); // Input includes padding --- 1 x 2 x 2 with 1 pixel of padding 
    input.Fill(0);
    input(1, 1, 0) = 2;
    input(1, 2, 0) = 1;
    input(1, 1, 1) = 3;
    input(1, 2, 1) = 2;
    Shape outputShape = { 1, 2, 2 }; // Output has no padding: 1 x 2 x 2
    LayerParameters parameters{ input, ZeroPadding(1), outputShape, NoPadding() };
    ConvolutionalParameters convolutionalParams{ 3, 1, ConvolutionMethod::diagonal, 2 };
    
    // Filter weights in `weightsVector` are in numFilters x numChannels x filterSize x filterSize order
    // clang-format off
    std::vector<ElementType> weightsVector {
        1, 3, 2,   3, 1, 1,   2, 3, 1,   // Filter 1, channel 1
        2, 4, 1,   3, 1, 2,   1, 4, 2,   // Filter 1, channel 2

        1, 2, 1,   2, 3, 2,   1, 2, 1,   // Filter 2, channel 1
        0, 3, 2,   3, 1, 2,   1, 0, 2 }; // Filter 2, channel 2
    // clang-format on

    // Filter weights in `weights` tensor are in numFilters x filterSize x filterSize x numChannels order
    TensorType weights(outputShape.NumChannels() * convolutionalParams.receptiveField, convolutionalParams.receptiveField, input.NumChannels());

    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape.NumChannels(); f++)
    {
        for (size_t k = 0; k < input.NumChannels(); k++)
        {
            for (size_t i = 0; i < convolutionalParams.receptiveField; i++)
            {
                for (size_t j = 0; j < convolutionalParams.receptiveField; j++)
                {
                    weights(f * convolutionalParams.receptiveField + i, j, k) = weightsVector[vectorIndex++];
                }
            }
        }
    }

    // Verify ConvolutionalLayer with simple method
    convolutionalParams.method = ConvolutionMethod::simple;
    ConvolutionalLayer<ElementType> convolutionalLayerSimple(parameters, convolutionalParams, weights);
    convolutionalLayerSimple.Compute();
    auto outputSimple = convolutionalLayerSimple.GetOutput();
    testing::ProcessTest("Testing ConvolutionalLayer (simple), values", Equals(outputSimple(0, 0, 0), 10) && Equals(outputSimple(0, 0, 1), 15) && Equals(outputSimple(0, 1, 0), 18) && Equals(outputSimple(0, 1, 1), 18));

    // Verify ConvolutionalLayer with unrolled method
    convolutionalParams.method = ConvolutionMethod::unrolled;
    ConvolutionalLayer<ElementType> convolutionalLayerUnrolled(parameters, convolutionalParams, weights);
    convolutionalLayerUnrolled.Compute();
    auto outputUnrolled = convolutionalLayerUnrolled.GetOutput();
    testing::ProcessTest("Testing ConvolutionalLayer (unrolled), values", Equals(outputUnrolled(0, 0, 0), 10) && Equals(outputUnrolled(0, 0, 1), 15) && Equals(outputUnrolled(0, 1, 0), 18) && Equals(outputUnrolled(0, 1, 1), 18));

    // Verify ConvolutionalLayer with diagonal method
    convolutionalParams.method = ConvolutionMethod::diagonal;
    ConvolutionalLayer<ElementType> convolutionalLayerDiagonal(parameters, convolutionalParams, weights);
    convolutionalLayerDiagonal.Compute();
    auto outputDiagonal = convolutionalLayerDiagonal.GetOutput();
    testing::ProcessTest("Testing ConvolutionalLayer (diagonal), values", Equals(outputDiagonal(0, 0, 0), 10) && Equals(outputDiagonal(0, 0, 1), 15) && Equals(outputDiagonal(0, 1, 0), 18) && Equals(outputDiagonal(0, 1, 1), 18));

}

template <typename ElementType>
void BinaryConvolutionalLayerGemmTest(ell::predictors::neural::BinaryWeightsScale scale)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Verify BinaryConvolutionalLayer with gemm method
    TensorType input(3, 4, 2); // Input includes padding
    input.Fill(0);
    input(1, 1, 0) = 2;
    input(1, 2, 0) = 1;
    input(1, 1, 1) = 3;
    input(1, 2, 1) = 2;
    Shape outputShape = { 1, 2, 2 }; // Output has no padding
    LayerParameters parameters{ input.GetReference(), ZeroPadding(1), outputShape, NoPadding() };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::gemm, scale };
    TensorType weights(convolutionalParams.receptiveField * outputShape.NumChannels(), convolutionalParams.receptiveField, input.NumChannels());
    // clang-format off
    std::vector<ElementType> weightsVector{   // RowMajor then depth order
        1, 3, 2, 3, 1, 1, 2, 3, 1,
        2, 4, 1, 3, 1, 2, 1, 4, 2,
        1, 2, 1, 2, 3, 2, 1, 2, 1,
        0, 3, 2, 3, 1, 2, 1, 0, 2 };
    // clang-format on
    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape.NumChannels(); f++)
    {
        for (size_t k = 0; k < input.NumChannels(); k++)
        {
            for (size_t i = 0; i < convolutionalParams.receptiveField; i++)
            {
                for (size_t j = 0; j < convolutionalParams.receptiveField; j++)
                {
                    weights(f * convolutionalParams.receptiveField + i, j, k) = weightsVector[vectorIndex++];
                }
            }
        }
    }

    BinaryConvolutionalLayer<ElementType> convolutionalLayer(parameters, convolutionalParams, weights);
    convolutionalLayer.Compute();
    auto output = convolutionalLayer.GetOutput();
    if (scale == ell::predictors::neural::BinaryWeightsScale::none)
    {
        testing::ProcessTest("Testing BinaryConvolutionalLayer (gemm) (no scaling), values", Equals(output(0, 0, 0), 4.0) && Equals(output(0, 0, 1), 4.0) && Equals(output(0, 1, 0), 4.0) && Equals(output(0, 1, 1), 4.0));
    }
    else
    {
        testing::ProcessTest("Testing BinaryConvolutionalLayer (gemm) (no scaling), values", Equals(output(0, 0, 0), 8.22222) && Equals(output(0, 0, 1), 6.44444) && Equals(output(0, 1, 0), 8.22222) && Equals(output(0, 1, 1), 6.44444));
    }

    // Verify that we can archive and unarchive the layer
    // Put the layer in a network so we can archive it
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    InputParameters inputParams = { { 1, 2, 2 }, { PaddingScheme::zeros, 0 }, { 3, 4, 2 }, { PaddingScheme::zeros, 0 }, 1 };
    auto inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);
    typename NeuralNetworkPredictor<ElementType>::Layers layers;
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BinaryConvolutionalLayer<ElementType>(parameters, convolutionalParams, weights)));
    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

    // archive the network
    utilities::SerializationContext context;
    NeuralNetworkPredictor<ElementType>::RegisterNeuralNetworkPredictorTypes(context);
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);
    archiver << neuralNetwork;

    // unarchive the network
    utilities::JsonUnarchiver unarchiver(strstream, context);
    NeuralNetworkPredictor<ElementType> archivedNetwork;
    unarchiver >> archivedNetwork;

    auto archivedOutput = neuralNetwork.Predict(DataVectorType{ 2, 1, 3, 2 });
    if (scale == ell::predictors::neural::BinaryWeightsScale::none)
    {
        testing::ProcessTest("Testing archived BinaryConvolutionalLayer (gemm) (no scaling), values", Equals(archivedOutput[0], 4.0) && Equals(archivedOutput[1], 4.0) && Equals(archivedOutput[2], 4.0) && Equals(archivedOutput[3], 4.0));
    }
    else
    {
        testing::ProcessTest("Testing archived BinaryConvolutionalLayer (gemm) (mean scaling), values", Equals(archivedOutput[0], 8.22222) && Equals(archivedOutput[1], 6.44444) && Equals(archivedOutput[2], 8.22222) && Equals(archivedOutput[3], 6.44444));
    }
}

template <typename ElementType>
void BinaryConvolutionalLayerGemmTest()
{
    BinaryConvolutionalLayerGemmTest<ElementType>(ell::predictors::neural::BinaryWeightsScale::mean);
    BinaryConvolutionalLayerGemmTest<ElementType>(ell::predictors::neural::BinaryWeightsScale::none);
}

template <typename ElementType>
void BinaryConvolutionalLayerBitwiseTest(ell::predictors::neural::BinaryWeightsScale scale)
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Verify BinaryConvolutionalLayer with gemm method
    TensorType input(3, 4, 2); // Input includes padding
    input.Fill(-1);
    input(1, 1, 0) = 2;
    input(1, 2, 0) = 1;
    input(1, 1, 1) = 3;
    input(1, 2, 1) = 2;
    Shape outputShape = { 1, 2, 2 }; // Output has no padding
    LayerParameters parameters{ input.GetReference(), MinusOnePadding(1), outputShape, NoPadding() };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::gemm, scale };
    TensorType weights(convolutionalParams.receptiveField * outputShape.NumChannels(), convolutionalParams.receptiveField, input.NumChannels());
    // clang-format off
    std::vector<ElementType> weightsVector{   // RowMajor then depth order
        1, 3, 2, 3, 1, 1, 2, 3, 1,
        2, 4, 1, 3, 1, 2, 1, 4, 2,
        1, 2, 1, 2, 3, 2, 1, 2, 1,
        0, 3, 2, 3, 1, 2, 1, 0, 2 };
    // clang-format on
    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape.NumChannels(); f++)
    {
        for (size_t k = 0; k < input.NumChannels(); k++)
        {
            for (size_t i = 0; i < convolutionalParams.receptiveField; i++)
            {
                for (size_t j = 0; j < convolutionalParams.receptiveField; j++)
                {
                    weights(f * convolutionalParams.receptiveField + i, j, k) = weightsVector[vectorIndex++];
                }
            }
        }
    }

    // Verify BinaryConvolutionalLayer with bitwise method. Since we're doing bitwise operations, change the padding scheme to be zeros.
    convolutionalParams.method = BinaryConvolutionMethod::bitwise;
    parameters.inputPaddingParameters.paddingScheme = PaddingScheme::zeros;
    input.Fill(0);
    input(1, 1, 0) = 2;
    input(1, 2, 0) = 1;
    input(1, 1, 1) = 3;
    input(1, 2, 1) = 2;

    BinaryConvolutionalLayer<ElementType> convolutionalLayer(parameters, convolutionalParams, weights);
    convolutionalLayer.Compute();
    auto output = convolutionalLayer.GetOutput();
    if (scale == ell::predictors::neural::BinaryWeightsScale::none)
    {
        testing::ProcessTest("Testing BinaryConvolutionalLayer (bitwise) (mean scaling), values", Equals(output(0, 0, 0), 4.0) && Equals(output(0, 0, 1), 4.0) && Equals(output(0, 1, 0), 4.0) && Equals(output(0, 1, 1), 4.0));
    }
    else
    {
        testing::ProcessTest("Testing BinaryConvolutionalLayer (bitwise) (no scaling), values", Equals(output(0, 0, 0), 8.22222) && Equals(output(0, 0, 1), 6.44444) && Equals(output(0, 1, 0), 8.22222) && Equals(output(0, 1, 1), 6.44444));
    }

    // Put the layer in a network so we can archive it
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    InputParameters inputParams = { { 1, 2, 2 }, { PaddingScheme::zeros, 0 }, { 3, 4, 2 }, { PaddingScheme::zeros, 0 }, 1 };
    auto inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);
    typename NeuralNetworkPredictor<ElementType>::Layers layers;
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BinaryConvolutionalLayer<ElementType>(parameters, convolutionalParams, weights)));
    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

    // archive the network
    utilities::SerializationContext context;
    NeuralNetworkPredictor<ElementType>::RegisterNeuralNetworkPredictorTypes(context);
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);
    archiver << neuralNetwork;

    // unarchive the network
    utilities::JsonUnarchiver unarchiver(strstream, context);
    NeuralNetworkPredictor<ElementType> archivedNetwork;
    unarchiver >> archivedNetwork;

    auto archivedOutput = neuralNetwork.Predict(DataVectorType{ 2, 1, 3, 2 });
    if (scale == ell::predictors::neural::BinaryWeightsScale::none)
    {
        testing::ProcessTest("Testing archived BinaryConvolutionalLayer (bitwise) (no scaling), values", Equals(archivedOutput[0], 4.0) && Equals(archivedOutput[1], 4.0) && Equals(archivedOutput[2], 4.0) && Equals(archivedOutput[3], 4.0));
    }
    else
    {
        testing::ProcessTest("Testing archived BinaryConvolutionalLayer (gemm) (mean scaling), values", Equals(archivedOutput[0], 8.22222) && Equals(archivedOutput[1], 6.44444) && Equals(archivedOutput[2], 8.22222) && Equals(archivedOutput[3], 6.44444));
    }
}

template <typename ElementType>
void BinaryConvolutionalLayerBitwiseTest()
{
    BinaryConvolutionalLayerBitwiseTest<ElementType>(ell::predictors::neural::BinaryWeightsScale::mean);
    BinaryConvolutionalLayerBitwiseTest<ElementType>(ell::predictors::neural::BinaryWeightsScale::none);
}

template <typename ElementType>
void SoftmaxLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    // Verify BiasLayer
    TensorType input(1, 1, 3);
    input(0, 0, 0) = 1;
    input(0, 0, 1) = 2;
    input(0, 0, 2) = 3;
    Shape outputShape = { 3, 3, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, ZeroPadding(1) };

    SoftmaxLayer<ElementType> softmaxLayer(parameters);
    softmaxLayer.Compute();
    auto output = softmaxLayer.GetOutput();
    testing::ProcessTest("Testing SoftmaxLayer, values", Equals(output(1, 1, 0), 0.0900305733) && Equals(output(1, 1, 1), 0.244728476) && Equals(output(1, 1, 2), 0.665240943));
    testing::ProcessTest("Testing SoftmaxLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 2, 0) == 0 && output(2, 2, 1) == 0);
}

template <typename ElementType>
void NeuralNetworkPredictorTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build an XOR net from previously trained values.
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 1, 1, 2 }, { PaddingScheme::zeros, 0 }, { 1, 1, 2 }, { PaddingScheme::zeros, 0 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), NoPadding(), { 1, 1, 3 }, NoPadding() };
    MatrixType weights1(3, 2);
    weights1(0, 0) = -0.97461396f;
    weights1(0, 1) = 1.40845299f;
    weights1(1, 0) = -0.14135513f;
    weights1(1, 1) = -0.54136097f;
    weights1(2, 0) = 0.99313086f;
    weights1(2, 1) = -0.99083692f;
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new FullyConnectedLayer<ElementType>(layerParameters, weights1)));

    layerParameters = { layers[0]->GetOutput(), NoPadding(), { 1, 1, 3 }, NoPadding() };
    VectorType bias1({ -0.43837756f, -0.90868396f, -0.0323102f });
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias1)));

    layerParameters = { layers[1]->GetOutput(), NoPadding(), { 1, 1, 3 }, NoPadding() };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ActivationLayer<ElementType, ReLUActivation>(layerParameters)));

    layerParameters = { layers[2]->GetOutput(), NoPadding(), { 1, 1, 1 }, NoPadding() };
    MatrixType weights2(1, 3);
    weights2(0, 0) = 1.03084767f;
    weights2(0, 1) = -0.10772263f;
    weights2(0, 2) = 1.04077697f;
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new FullyConnectedLayer<ElementType>(layerParameters, weights2)));

    layerParameters = { layers[3]->GetOutput(), NoPadding(), { 1, 1, 1 }, NoPadding() };
    VectorType bias2({ 1.40129846e-20f });
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias2)));

    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    std::vector<ElementType> output;

    // Check  the result for the 4 permutations of input. This validates that:
    // - the weights loaded correctly.
    // - the operations in each layer are working correctly
    // - the feed forward logic is working correctly

    output = neuralNetwork.Predict(DataVectorType({ 0, 0 }));
    testing::ProcessTest("Testing NeuralNetworkPredictor, Predict of XOR net for 0 0 ", Equals(output[0], 0.0));

    output = neuralNetwork.Predict(DataVectorType({ 0, 1 }));
    testing::ProcessTest("Testing NeuralNetworkPredictor, Predict of XOR net for 0 1 ", Equals(output[0], 1.0));

    output = neuralNetwork.Predict(DataVectorType({ 1, 0 }));
    testing::ProcessTest("Testing NeuralNetworkPredictor, Predict of XOR net for 1 0 ", Equals(output[0], 1.0));

    output = neuralNetwork.Predict(DataVectorType({ 1, 1 }));
    testing::ProcessTest("Testing NeuralNetworkPredictor, Predict of XOR net for 1 1 ", Equals(output[0], 0.0));

    // Verify that we can archive and unarchive the predictor
    utilities::SerializationContext context;
    NeuralNetworkPredictor<ElementType>::RegisterNeuralNetworkPredictorTypes(context);
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);
    archiver << neuralNetwork;
    utilities::JsonUnarchiver unarchiver(strstream, context);

    NeuralNetworkPredictor<ElementType> neuralNetwork2;
    unarchiver >> neuralNetwork2;

    output = neuralNetwork2.Predict(DataVectorType({ 0, 0 }));
    testing::ProcessTest("Testing NeuralNetworkPredictor from archive, Predict of XOR net for 0 0 ", Equals(output[0], 0.0));

    output = neuralNetwork2.Predict(DataVectorType({ 0, 1 }));
    testing::ProcessTest("Testing NeuralNetworkPredictor from archive, Predict of XOR net for 0 1 ", Equals(output[0], 1.0));

    output = neuralNetwork2.Predict(DataVectorType({ 1, 0 }));
    testing::ProcessTest("Testing NeuralNetworkPredictor from archive, Predict of XOR net for 1 0 ", Equals(output[0], 1.0));

    output = neuralNetwork2.Predict(DataVectorType({ 1, 1 }));
    testing::ProcessTest("Testing NeuralNetworkPredictor from archive, Predict of XOR net for 1 1 ", Equals(output[0], 0.0));

    // Remove the last 2 layers, (Dense and Bias)
    neuralNetwork2.RemoveLastLayers(2);
    output = neuralNetwork2.Predict(DataVectorType({ 0, 1 }));
    testing::ProcessTest("Testing cut NeuralNetworkPredictor, predict for 0 1 ", Equals(output[0], 0.970072031) && Equals(output[1], 0.0) && Equals(output[2], 0.0));
}

// clang-format off
const float uData[] = { -0.306974f, -0.314942f, -0.307079f, -0.0778356f, -0.0929513f, 0.0426045f, -0.0200071f,
                        0.508866f, 0.525531f, 0.345996f, -0.633406f, -0.519455f, 0.617442f, -0.0790342f,
                        2.13148f, 2.61342f, -2.99549f, -6.15958f, 0.224837f, 0.0745432f, 0.154865f };
const float rData[] = { -0.438305f, -0.438798f, -0.509791f, 0.385411f, -0.210201f, -0.302488f, 0.0717234f,
                        0.259852f, 0.532692f, 0.675258f, 0.0314993f, -0.609884f, -0.419196f, 0.407534f,
                        0.221932f, 0.51503f, -0.278936f, 0.673416f, 0.307534f, -0.176314f, 0.440408f };
const float hData[] = { 0.0364258f, 0.557955f, -0.467648f, 0.265914f, 0.343273f, -0.0306102f, -0.265686f,
                        0.241587f, 0.283854f, 0.232303f, -0.397746f, -0.191887f, -0.0618932f, -0.551409f,
                        0.847701f, 0.234382f, -0.107097f, -0.38192f, 0.074817f, 0.555262f, 0.479104f };
// clang-format on

template <typename ElementType>
void GRULayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType updateBias = VectorType({ 0.0f, 0.0f, 3.95111f });
    VectorType resetBias = VectorType({ 0.0f, 0.0f, 0.0f });
    VectorType hiddenBias = VectorType({ -0.0686757f, 0.0f, 0.281977f });

    MatrixType updateWeights(3, 7);
    MatrixType resetWeights(3, 7);
    MatrixType hiddenWeights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            updateWeights(i, j) = uData[columnIndex];
            resetWeights(i, j) = rData[columnIndex];
            hiddenWeights(i, j) = hData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output ~1,0,0
    input(0, 0, 0) = static_cast<ElementType>(5.1);
    input(0, 0, 1) = static_cast<ElementType>(3.5);
    input(0, 0, 2) = static_cast<ElementType>(1.4);
    input(0, 0, 3) = static_cast<ElementType>(0.2);

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };

    GRUParameters<ElementType> gruParams{ updateWeights, resetWeights, hiddenWeights, updateBias, resetBias, hiddenBias };
    GRULayer<ElementType, TanhActivation, SigmoidActivation> gru(parameters, gruParams);
    gru.Compute();
    TensorType output = gru.GetOutput();

    testing::ProcessTest("Testing GRULayer, values", Equals(output(0, 0, 0), 0.861001074314117) && Equals(output(0, 0, 1), 0.008108692243695) && Equals(output(0, 0, 2), 0.000000000000000));
}

// clang-format off
const float iData[] = { 0.739646f, 0.8501f, -2.15136f, -2.44612f, 0.0639512f, -0.0492275f, 0.167204f,
                        -0.49359f, 0.253341f, -0.239276f, 0.114082f, -0.360225f, 0.434314f, -0.28489f,
                        -0.573704f, -0.0273829f, 0.0242156f, -0.600619f, -0.258574f, -0.312928f, -0.0446059f };
const float fData[] = { 0.0628231f, 0.145727f, -0.258802f, -0.57547f, -0.511279f, -0.470488f, 0.231888f,
                        0.42041f, -0.440816f, -0.343813f, 0.463799f, -0.456978f, 0.081054f, 0.532126f,
                        0.51855f, -0.123881f, 0.509249f, 0.324012f, 0.318677f, -0.411882f, 0.082f };
const float cData[] = { 0.187203f, 0.863434f, 0.490011f, -0.216801f, -0.290302f, 0.338456f, -0.216217f,
                        -0.000121037f, 0.0000392739f, 0.00000052499f, 0.0000676336f, 0.196989f, 0.312441f, 0.355654f,
                        0.468885f, -0.236218f, 0.415782f, 0.302927f, -0.0503453f, -0.183221f, -0.500112f };
const float oData[] = { 0.517059f, 0.470772f, -0.919974f, -0.319515f, 0.224966f, 0.195129f, 0.306053f,
                        0.261489f, 0.499691f, 0.132338f, 0.47862f, 0.21803f, 0.00246173f, -0.0274337f,
                        -0.385968f, 0.120127f, -0.360038f, -0.21129f, 0.0611264f, -0.17212f, -0.165724f };
// clang-format on
template <typename ElementType>
void LSTMLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType inputBias = VectorType({ 0.747351f, -0.112848f, 0.0f });
    VectorType forgetMeBias = VectorType({ 1.0f, 1.0f, 1.0f });
    VectorType candidateBias = VectorType({ 0.733668f, 0.000431956f, 0.0f });
    VectorType outputBias = VectorType({ 0.385433f, 0.0f, 0.0f });

    MatrixType inputWeights(3, 7);
    MatrixType forgetMeWeights(3, 7);
    MatrixType candidateWeights(3, 7);
    MatrixType outputWeights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            inputWeights(i, j) = iData[columnIndex];
            forgetMeWeights(i, j) = fData[columnIndex];
            candidateWeights(i, j) = cData[columnIndex];
            outputWeights(i, j) = oData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output 1,0,0
    input(0, 0, 0) = static_cast<ElementType>(5.1);
    input(0, 0, 1) = static_cast<ElementType>(3.5);
    input(0, 0, 2) = static_cast<ElementType>(1.4);
    input(0, 0, 3) = static_cast<ElementType>(0.2);

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };

    LSTMParameters<ElementType> lstmParams{ inputWeights, forgetMeWeights, candidateWeights, outputWeights, inputBias, forgetMeBias, candidateBias, outputBias };

    LSTMLayer<ElementType, TanhActivation, SigmoidActivation> lstm(parameters, lstmParams);
    lstm.Compute();
    TensorType output = lstm.GetOutput();

    testing::ProcessTest("Testing LSTMLayer, values", Equals(output(0, 0, 0), 0.7275221943855286) && Equals(output(0, 0, 1), -0.0000036868595998) && Equals(output(0, 0, 2), 0.0045761126093566));
}

// clang-format off
const float wData[] = { 0.0381341f, 0.55826f, -0.467607f, 0.264272f, -0.733331f, 0.464226f, 0.496708f,
                        0.0581872f, -0.514144f, 0.702823f, -1.50401f, 0.373703f, 0.885559f, -0.27592f,
                        -0.116469f, 0.320376f, -0.534044f, 1.92602f, -0.567954f, -0.0167191f, -0.822891f };
// clang-format on

template <typename ElementType>
void RecurrentLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;

    VectorType biases = VectorType({ -0.0773237f, 0.909263f, -0.297635f });

    MatrixType weights(3, 7);

    int columnIndex = 0;

    // transform our weights into 3 x 7 matrices (21 values)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            weights(i, j) = wData[columnIndex];

            columnIndex++;
        }
    }

    TensorType input(1, 1, 4);

    // should output ~ 1,1,0
    input(0, 0, 0) = static_cast<ElementType>(5.1);
    input(0, 0, 1) = static_cast<ElementType>(3.5);
    input(0, 0, 2) = static_cast<ElementType>(1.4);
    input(0, 0, 3) = static_cast<ElementType>(0.2);

    Shape outputShape = { 1, 1, 3 };
    LayerParameters parameters{ input, NoPadding(), outputShape, NoPadding() };

    RecurrentLayer<ElementType, TanhActivation> recurrent(parameters, weights, biases);
    recurrent.Compute();
    TensorType output = recurrent.GetOutput();

    testing::ProcessTest("Testing RNN, values", Equals(output(0, 0, 0), 0.899439096450806) && Equals(output(0, 0, 1), 0.089424349367619) && Equals(output(0, 0, 2), -0.131993845105171));
}

template <typename ElementType>
void FillTensor(ell::math::ChannelColumnRowTensor<ElementType>& tensor, int startValue = 0)
{
    int val = startValue;
    tensor.Generate([&val]() { return val++; });
}

template <typename ElementType>
void FillVector(ell::math::ColumnVector<ElementType>& vector, int startValue = 0)
{
    int val = startValue;
    vector.Generate([&val]() { return val++; });
}

template <typename ElementType>
void ConvolutionalArchiveTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 3, 3, 3 }, { PaddingScheme::zeros, 0 }, { 5, 5, 3 }, { PaddingScheme::zeros, 1 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), { PaddingScheme::zeros, 1 }, { 3, 3, 8 }, NoPadding() };
    auto convolutionMethod = ConvolutionMethod::unrolled;
    ConvolutionalParameters convolutionalParams{ 3, 1, convolutionMethod, 1 };
    TensorType convWeights1(8 * 3, 3, 3);
    FillTensor(convWeights1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new ConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights1)));

    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    std::vector<double> input(3 * 3 * 3);
    int val = 0;
    std::generate(input.begin(), input.end(), [&val]() { return val++; });

    utilities::SerializationContext context;
    NeuralNetworkPredictor<ElementType>::RegisterNeuralNetworkPredictorTypes(context);
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);
    archiver << neuralNetwork;

    utilities::JsonUnarchiver unarchiver(strstream, context);
    NeuralNetworkPredictor<ElementType> neuralNetwork2;
    unarchiver >> neuralNetwork2;
    auto output = neuralNetwork.Predict(DataVectorType(input));
    auto output2 = neuralNetwork2.Predict(DataVectorType(input));
    testing::ProcessTest("Testing Convolutional predictor from archive", testing::IsEqual(output, output2));
}

template <typename ElementType>
void BinaryConvolutionalArchiveTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 3, 3, 3 }, { PaddingScheme::zeros, 0 }, { 5, 5, 3 }, { PaddingScheme::zeros, 1 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), { PaddingScheme::zeros, 1 }, { 3, 3, 8 }, NoPadding() };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::bitwise, BinaryWeightsScale::mean };
    TensorType convWeights1(8 * 3, 3, 3);
    FillTensor(convWeights1);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BinaryConvolutionalLayer<ElementType>(layerParameters, convolutionalParams, convWeights1)));

    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    std::vector<double> input(3 * 3 * 3);
    int val = 0;
    std::generate(input.begin(), input.end(), [&val]() { return val++; });

    utilities::SerializationContext context;
    NeuralNetworkPredictor<ElementType>::RegisterNeuralNetworkPredictorTypes(context);
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);
    archiver << neuralNetwork;

    utilities::JsonUnarchiver unarchiver(strstream, context);
    NeuralNetworkPredictor<ElementType> neuralNetwork2;
    unarchiver >> neuralNetwork2;
    auto output = neuralNetwork.Predict(DataVectorType(input));
    auto output2 = neuralNetwork2.Predict(DataVectorType(input));
    testing::ProcessTest("Testing Binary convolutional predictor from archive", testing::IsEqual(output, output2));
}
