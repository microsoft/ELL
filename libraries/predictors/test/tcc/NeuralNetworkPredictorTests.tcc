////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorTests.tcc (predictors_test)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LeakyReLUActivation.h"
#include "MaxPoolingFunction.h"
#include "NeuralNetworkPredictor.h"
#include "ReLUActivation.h"

// testing
#include "testing.h"

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
    T0(0, 0, 0) = 1.0;
    T0(0, 1, 0) = -2.0;
    T0(1, 0, 1) = 3.0;
    T0(1, 1, 1) = -4.0;

    TensorType T1(2, 2, 2);

    auto relu = ReLUActivation<ElementType>();
    for (size_t i = 0; i < T0.NumRows(); ++i)
    {
        for (size_t j = 0; j < T0.NumColumns(); ++j)
        {
            for (size_t k = 0; k < T0.NumChannels(); ++k)
            {
                T1(i, j, k) = relu.Apply(T0(i, j, k));
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
                T1(i, j, k) = leakyRelu.Apply(T0(i, j, k));
            }
        }
    }
    testing::ProcessTest("Testing LeakyReLUActivation", Equals(T1(0, 0, 0), 1.0) && Equals(T1(0, 1, 0), -0.2) && Equals(T1(1, 0, 1), 3.0) && Equals(T1(1, 1, 1), -0.4));
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

    BatchNormalizationLayer<ElementType> bnLayer(bnParameters, mean, variance, 1e-6, EpsilonSummand::SqrtVariance);
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
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    // Verify Input
    Shape inputShape = { 2, 2, 2 };
    Shape outputShape = { 4, 4, 2 };
    typename InputLayer<ElementType>::InputParameters parameters{ inputShape, NoPadding(), outputShape, ZeroPadding(1), 2.0 };

    InputLayer<ElementType> inputLayer(parameters);
    inputLayer.SetInput({ 1, 2, 3, 4, 5, 6, 7, 8 });
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
    using VectorType = typename Layer<ElementType>::VectorType;

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

    // Verify BatchNormailzationLayer
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
}

template <typename ElementType>
void ConvolutionalLayerTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;

    // Verify ConvolutionalLayer with diagonal method
    TensorType input(3, 4, 2); // Input includes padding
    input.Fill(0);
    input(1, 1, 0) = 2;
    input(1, 2, 0) = 1;
    input(1, 1, 1) = 3;
    input(1, 2, 1) = 2;
    Shape outputShape = { 1, 2, 2 }; // Output has no padding
    LayerParameters parameters{ input, ZeroPadding(1), outputShape, NoPadding() };
    ConvolutionalParameters convolutionalParams{ 3, 1, ConvolutionMethod::diagonal, 2 };
    TensorType weights(convolutionalParams.receptiveField * outputShape[2], convolutionalParams.receptiveField, input.NumChannels());
    // clang-format off
    std::vector<ElementType> weightsVector{   // RowMajor then depth order
        1, 3, 2, 3, 1, 1, 2, 3, 1,
        2, 4, 1, 3, 1, 2, 1, 4, 2,
        1, 2, 1, 2, 3, 2, 1, 2, 1,
        0, 3, 2, 3, 1, 2, 1, 0, 2 };
    // clang-format on
    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape[2]; f++)
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

    ConvolutionalLayer<ElementType> convolutionalLayer(parameters, convolutionalParams, weights);
    convolutionalLayer.Compute();
    auto output = convolutionalLayer.GetOutput();

    testing::ProcessTest("Testing ConvolutionalLayer (diagonal), values", Equals(output(0, 0, 0), 10) && Equals(output(0, 0, 1), 15) && Equals(output(0, 1, 0), 18) && Equals(output(0, 1, 1), 18));

    // Verify ConvolutionalLayer with regular method
    convolutionalParams.method = ConvolutionMethod::columnwise;
    ConvolutionalLayer<ElementType> convolutionalLayer2(parameters, convolutionalParams, weights);
    convolutionalLayer2.Compute();
    auto output2 = convolutionalLayer2.GetOutput();

    testing::ProcessTest("Testing ConvolutionalLayer (columnwise), values", Equals(output2(0, 0, 0), 10) && Equals(output2(0, 0, 1), 15) && Equals(output2(0, 1, 0), 18) && Equals(output2(0, 1, 1), 18));
}

template <typename ElementType>
void BinaryConvolutionalLayerGemmTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Verify BinaryConvolutionalLayer with gemm method
    TensorType input(3, 4, 2); // Input includes padding
    input.Fill(0);
    input(1, 1, 0) = 2;
    input(1, 2, 0) = 1;
    input(1, 1, 1) = 3;
    input(1, 2, 1) = 2;
    Shape outputShape = { 1, 2, 2 }; // Output has no padding
    LayerParameters parameters{ input.GetReference(), MinusOnePadding(1), outputShape, NoPadding() };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::gemm };
    TensorType weights(convolutionalParams.receptiveField * outputShape[2], convolutionalParams.receptiveField, input.NumChannels());
    // clang-format off
    std::vector<ElementType> weightsVector{   // RowMajor then depth order
        1, 3, 2, 3, 1, 1, 2, 3, 1,
        2, 4, 1, 3, 1, 2, 1, 4, 2,
        1, 2, 1, 2, 3, 2, 1, 2, 1,
        0, 3, 2, 3, 1, 2, 1, 0, 2 };
    // clang-format on
    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape[2]; f++)
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
    testing::ProcessTest("Testing BinaryConvolutionalLayer (gemm), values", Equals(output(0, 0, 0), -20.5555553) && Equals(output(0, 0, 1), -9.66666603) && Equals(output(0, 1, 0), -20.5555553) && Equals(output(0, 1, 1), -9.66666603));

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
    testing::ProcessTest("Testing archived BinaryConvolutionalLayer (gemm), values", Equals(archivedOutput[0], -20.5555553) && Equals(archivedOutput[1], -9.66666603) && Equals(archivedOutput[2], -20.5555553) && Equals(archivedOutput[3], -9.66666603));
}

template <typename ElementType>
void BinaryConvolutionalLayerBitwiseTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
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
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::gemm };
    TensorType weights(convolutionalParams.receptiveField * outputShape[2], convolutionalParams.receptiveField, input.NumChannels());
    // clang-format off
    std::vector<ElementType> weightsVector{   // RowMajor then depth order
        1, 3, 2, 3, 1, 1, 2, 3, 1,
        2, 4, 1, 3, 1, 2, 1, 4, 2,
        1, 2, 1, 2, 3, 2, 1, 2, 1,
        0, 3, 2, 3, 1, 2, 1, 0, 2 };
    // clang-format on
    size_t vectorIndex = 0;
    for (size_t f = 0; f < outputShape[2]; f++)
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
    testing::ProcessTest("Testing BinaryConvolutionalLayer (bitwise), values", Equals(output(0, 0, 0), -20.5555553) && Equals(output(0, 0, 1), -9.66666603) && Equals(output(0, 1, 0), -20.5555553) && Equals(output(0, 1, 1), -9.66666603));

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
    testing::ProcessTest("Testing archived BinaryConvolutionalLayer (bitwise), values", Equals(archivedOutput[0], -20.5555553) && Equals(archivedOutput[1], -9.66666603) && Equals(archivedOutput[2], -20.5555553) && Equals(archivedOutput[3], -9.66666603));
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
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
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
}

template <typename ElementType>
void FillTensor(ell::math::ChannelColumnRowTensor<ElementType>& tensor, int startValue = 0)
{
    int val = 0;
    tensor.Generate([&val]() { return val++; });
}

template <typename ElementType>
void FillVector(ell::math::ColumnVector<ElementType>& vector, int startValue = 0)
{
    int val = 0;
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
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 3, 3, 3 }, { PaddingScheme::zeros, 0 }, { 5, 5, 3 }, { PaddingScheme::zeros, 1 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), { PaddingScheme::zeros, 1 }, { 3, 3, 8 }, NoPadding() };
    auto convolutionMethod = ConvolutionMethod::columnwise;
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
    using Shape = typename Layer<ElementType>::Shape;
    using VectorType = typename Layer<ElementType>::VectorType;
    using MatrixType = typename Layer<ElementType>::MatrixType;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 3, 3, 3 }, { PaddingScheme::zeros, 0 }, { 5, 5, 3 }, { PaddingScheme::zeros, 1 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters{ inputLayer->GetOutput(), { PaddingScheme::zeros, 1 }, { 3, 3, 8 }, NoPadding() };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::bitwise };
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
