////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkLayerNodesTest.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NeuralNetworkLayerNodesTest.h"

// nodes
#include "ActivationLayerNode.h"
#include "BatchNormalizationLayerNode.h"
#include "BiasLayerNode.h"
#include "BinaryConvolutionalLayerNode.h"
#include "ConvolutionalLayerNode.h"
#include "FullyConnectedLayerNode.h"
#include "NeuralNetworkPredictorNode.h"
#include "PoolingLayerNode.h"
#include "ScalingLayerNode.h"
#include "SoftmaxLayerNode.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "Node.h"

// predictors
#include "NeuralNetworkPredictor.h"

// predictors/neural
#include "ActivationLayer.h"
#include "BatchNormalizationLayer.h"
#include "BiasLayer.h"
#include "BinaryConvolutionalLayer.h"
#include "ConvolutionalLayer.h"
#include "FullyConnectedLayer.h"
#include "InputLayer.h"
#include "PoolingLayer.h"
#include "ReLUActivation.h"
#include "RegionDetectionLayer.h"
#include "ScalingLayer.h"
#include "SoftmaxLayer.h"

// testing
#include "testing.h"

// common
#include "LoadModel.h" // for RegisterNodeTypes

// stl
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace ell;
using namespace nodes;

//
// Helpers
//
namespace
{
size_t GetShapeSize(const math::IntegerTriplet& shape)
{
    return shape[0] * shape[1] * shape[2];
}
}

//
// Neural network predictor node
//
template <typename ElementType>
ell::predictors::NeuralNetworkPredictor<ElementType> CreateNeuralNetworkPredictor()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using VectorType = typename Layer<ElementType>::VectorType;

    // Build a net
    typename NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename NeuralNetworkPredictor<ElementType>::Layers layers;

    InputParameters inputParams = { { 1, 1, 3 }, { PaddingScheme::zeros, 0 }, { 1, 1, 3 }, { PaddingScheme::zeros, 0 }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParameters = { inputLayer->GetOutput(), NoPadding(), { 1, 1, 3 }, NoPadding() };
    layerParameters = { inputLayer->GetOutput(), NoPadding(), { 1, 1, 3 }, NoPadding() };
    VectorType bias1({ -0.43837756f, -0.90868396f, -0.0323102f });
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BiasLayer<ElementType>(layerParameters, bias1)));
    NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));
    return neuralNetwork;
}

static void TestNeuralNetworkPredictorNode()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Get a net
    NeuralNetworkPredictor<ElementType> neuralNetwork = CreateNeuralNetworkPredictor<ElementType>();

    std::vector<ElementType> input = { 0, 1, 2 };
    auto output = neuralNetwork.Predict(DataVectorType(input));

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<double>>(inputNode->output, neuralNetwork);

    inputNode->SetInput(input);
    auto modelOutput = model.ComputeOutput(predictorNode->output);
    testing::ProcessTest("Testing NeuralNetworkPredictorNode compute", testing::IsEqual(modelOutput, output));
}

static void TestArchiveNeuralNetworkPredictorNode()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Get a net
    NeuralNetworkPredictor<ElementType> neuralNetwork = CreateNeuralNetworkPredictor<ElementType>();

    std::vector<ElementType> input = { 0, 1, 2 };
    auto output = neuralNetwork.Predict(DataVectorType(input));

    // Create model
    model::Model model;
    {
        auto inputNode = model.AddNode<model::InputNode<ElementType>>(GetShapeSize(neuralNetwork.GetInputShape()));
        auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, neuralNetwork);
        UNUSED(predictorNode);
    }
    auto numNodes = model.Size();

    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);

    // Archive the model
    archiver << model;

    // Unarchive the model
    utilities::JsonUnarchiver unarchiver(strstream, context);
    model::Model model2;
    unarchiver >> model2;

    testing::ProcessTest("Testing NeuralNetworkPredictorNode archive (model size)", testing::IsEqual(model2.Size(), numNodes));

    auto inputNodes = model2.GetNodesByType<model::InputNode<ElementType>>();
    auto predictorNodes = model2.GetNodesByType<nodes::NeuralNetworkPredictorNode<ElementType>>();
    testing::ProcessTest("Testing NeuralNetworkPredictorNode archive (input node)", testing::IsEqual((int)inputNodes.size(), 1));
    testing::ProcessTest("Testing NeuralNetworkPredictorNode archive (predictor node)", testing::IsEqual((int)predictorNodes.size(), 1));

    auto inputNode2 = inputNodes[0];
    inputNode2->SetInput(input);
    auto predictorNode2 = predictorNodes[0];
    auto modelOutput = model.ComputeOutput(predictorNode2->output);

    const double eps = 1e-6;
    testing::ProcessTest("Testing NeuralNetworkPredictorNode archive (compute)", testing::IsEqual(modelOutput, output, eps));
}

static void TestArchiveNeuralNetworkLayerNodes()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    using ElementType = double;
    using DataVectorType = typename NeuralNetworkPredictor<ElementType>::DataVectorType;

    // Get a net
    NeuralNetworkPredictor<ElementType> neuralNetwork = CreateNeuralNetworkPredictor<ElementType>();

    std::vector<ElementType> input = { 0, 1, 2 };
    auto output = neuralNetwork.Predict(DataVectorType(input));

    // Create a model
    model::Model model;
    {
        auto inputNode = model.AddNode<model::InputNode<ElementType>>(GetShapeSize(neuralNetwork.GetInputShape()));
        auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, neuralNetwork);
        UNUSED(predictorNode);
    }

    // Refine the model
    model::TransformContext transformContext;
    model::ModelTransformer transformer;
    auto refinedModel = transformer.RefineModel(model, transformContext, 1);
    auto numNodes = refinedModel.Size();

    // Archive the model
    utilities::SerializationContext context;
    common::RegisterNodeTypes(context);
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);

    archiver << refinedModel;

    // Unarchive the model
    NeuralNetworkPredictor<ElementType>::RegisterNeuralNetworkPredictorTypes(context);
    utilities::JsonUnarchiver unarchiver(strstream, context);
    model::Model model2;
    unarchiver >> model2;

    testing::ProcessTest("Testing NeuralNetworkLayerNodes archive (model size)", testing::IsEqual(model2.Size(), numNodes));

    auto inputNodes = model2.GetNodesByType<model::InputNode<ElementType>>();
    testing::ProcessTest("Testing NeuralNetworkLayerNodes archive (input node)", testing::IsEqual((int)inputNodes.size(), 1));
}

//
// Individual layer nodes
//

static void TestActivationLayerNode()
{
    using ElementType = double;
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;
    const double eps = 1e-6;

    // Verify ActivationLayer
    TensorType input(2, 2, 2);
    input(0, 0, 0) = 1.0;
    input(0, 1, 0) = -2.0;
    input(1, 0, 1) = 3.0;
    input(1, 1, 1) = -4.0;
    Shape activationOutputShape = { 4, 4, 2 };
    LayerParameters activationParameters{ input, NoPadding(), activationOutputShape, ZeroPadding(1) };

    ActivationLayer<ElementType, ReLUActivation> activationLayer(activationParameters);
    activationLayer.Compute();
    auto output = activationLayer.GetOutput();
    testing::ProcessTest("Testing ActivationLayer, values", output(1, 1, 0) == 1.0 && output(1, 2, 0) == 0 && output(2, 1, 1) == 3.0 && output(2, 2, 1) == 0);
    testing::ProcessTest("Testing ActivationLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(input.Size());
    auto activationNode = model.AddNode<nodes::ActivationLayerNode<double, ReLUActivation>>(inputNode->output, activationLayer);

    inputNode->SetInput(input.ToArray());
    auto modelOutput = model.ComputeOutput(activationNode->output);
    testing::ProcessTest("Testing ActivationLayerNode compute", testing::IsEqual(modelOutput, output.ToArray(), eps));
}

static void TestBatchNormalizationLayerNode()
{
    using LayerType = predictors::neural::BatchNormalizationLayer<double>;

    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;

    // Verify BatchNormalizationLayer
    TensorType input(2, 2, 2);
    input(0, 0, 0) = 11;
    input(0, 1, 0) = 7;
    input(1, 0, 1) = 30;
    input(1, 1, 1) = 50;
    Shape outputShape = { 4, 4, 2 };
    LayerParameters bnParameters{ input, predictors::neural::NoPadding(), outputShape, predictors::neural::ZeroPadding(1) };
    VectorType mean({ 5, 10 });
    VectorType variance({ 4.0, 16.0 });

    predictors::neural::BatchNormalizationLayer<double> bnLayer(bnParameters, mean, variance, 1.0e-6f, predictors::neural::EpsilonSummand::SqrtVariance);
    bnLayer.Compute();
    auto output = bnLayer.GetOutput();

    double eps = 1e-5;
    testing::ProcessTest("Testing BatchNormalizationLayer, values",
                         testing::IsEqual(output(1, 1, 0), 3.0, eps) &&
                             testing::IsEqual(output(1, 2, 0), 1.0, eps) &&
                             testing::IsEqual(output(2, 1, 1), 5.0, eps) &&
                             testing::IsEqual(output(2, 2, 1), 10.0, eps));
    testing::ProcessTest("Testing BatchNormalizationLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(input.Size());
    auto bnNode = model.AddNode<nodes::BatchNormalizationLayerNode<double>>(inputNode->output, bnLayer);

    inputNode->SetInput(input.ToArray());
    auto modelOutput = model.ComputeOutput(bnNode->output);
    testing::ProcessTest("Testing BatchNormalizationLayerNode compute", testing::IsEqual(modelOutput, output.ToArray(), eps));
}

static void TestBiasLayerNode()
{
    using LayerType = predictors::neural::BiasLayer<double>;

    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;

    // Set up bias layer
    TensorType input(2, 2, 2);
    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;
    Shape outputShape = { 4, 4, 2 };
    LayerParameters parameters{ input, predictors::neural::NoPadding(), outputShape, predictors::neural::ZeroPadding(1) };
    VectorType bias({ 5, 10 });

    predictors::neural::BiasLayer<double> biasLayer(parameters, bias);
    biasLayer.Compute();
    auto output = biasLayer.GetOutput();
    testing::ProcessTest("Testing BiasLayer, values",
                         testing::IsEqual(output(1, 1, 0), 6.0) &&
                             testing::IsEqual(output(1, 2, 0), 7.0) &&
                             testing::IsEqual(output(2, 1, 1), 13.0) &&
                             testing::IsEqual(output(2, 2, 1), 14.0));
    testing::ProcessTest("Testing BiasLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(input.Size());
    auto biasNode = model.AddNode<nodes::BiasLayerNode<double>>(inputNode->output, biasLayer);

    inputNode->SetInput(input.ToArray());
    auto modelOutput = model.ComputeOutput(biasNode->output);
    testing::ProcessTest("Testing BiasLayerNode compute", testing::IsEqual(modelOutput, output.ToArray()));
}

static void TestConvolutionalLayerNode()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

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

    //
    // Verify ConvolutionalLayer with diagonal method
    //
    ConvolutionalLayer<ElementType> layer1(parameters, convolutionalParams, weights);
    layer1.Compute();
    layer1.Compute();
    auto output1 = layer1.GetOutput();
    ElementType eps = 1e-6;
    ElementType v1 = 10;
    ElementType v2 = 15;
    ElementType v3 = 18;
    ElementType v4 = 18;
    testing::ProcessTest("Testing ConvolutionalLayer (diagonal), values",
                         testing::IsEqual(output1(0, 0, 0), v1, eps) &&
                             testing::IsEqual(output1(0, 0, 1), v2, eps) &&
                             testing::IsEqual(output1(0, 1, 0), v3, eps) &&
                             testing::IsEqual(output1(0, 1, 1), v4, eps));

    // Create model
    model::Model model1;
    auto inputNode1 = model1.AddNode<model::InputNode<double>>(input.Size());
    auto computeNode1 = model1.AddNode<nodes::ConvolutionalLayerNode<double>>(inputNode1->output, layer1);

    inputNode1->SetInput(input.ToArray());
    auto modelOutput1 = model1.ComputeOutput(computeNode1->output);
    testing::ProcessTest("Testing ConvolutionalLayer (diagonal) compute", testing::IsEqual(modelOutput1, output1.ToArray()));

    //
    // Verify ConvolutionalLayer with regular method
    //
    convolutionalParams.method = ConvolutionMethod::unrolled;
    ConvolutionalLayer<ElementType> layer2(parameters, convolutionalParams, weights);
    layer2.Compute();
    auto output2 = layer2.GetOutput();

    testing::ProcessTest("Testing ConvolutionalLayer (regular), values",
                         (testing::IsEqual(output2(0, 0, 0), v1, eps) &&
                          testing::IsEqual(output2(0, 0, 1), v2, eps) &&
                          testing::IsEqual(output2(0, 1, 0), v3, eps) &&
                          testing::IsEqual(output2(0, 1, 1), v4, eps)));
    // Create model
    model::Model model2;
    auto inputNode2 = model2.AddNode<model::InputNode<double>>(input.Size());
    auto computeNode2 = model2.AddNode<nodes::ConvolutionalLayerNode<double>>(inputNode2->output, layer2);

    inputNode2->SetInput(input.ToArray());
    auto modelOutput2 = model2.ComputeOutput(computeNode2->output);
    testing::ProcessTest("Testing ConvolutionalLayer (regular) compute", testing::IsEqual(modelOutput2, output2.ToArray()));
}

static void TestBinaryConvolutionalLayerNode()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    //
    // Verify BinaryConvolutionalLayer with gemm method
    //
    TensorType input(3, 4, 2); // Input includes padding
    input.Fill(-1);
    input(1, 1, 0) = 2;
    input(1, 2, 0) = 1;
    input(1, 1, 1) = 3;
    input(1, 2, 1) = 2;
    Shape outputShape = { 1, 2, 2 }; // Output has no padding
    LayerParameters parameters{ input, MinusOnePadding(1), outputShape, NoPadding() };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::gemm, BinaryWeightsScale::mean };
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

    BinaryConvolutionalLayer<ElementType> layer1(parameters, convolutionalParams, weights);
    layer1.Compute();
    auto output1 = layer1.GetOutput();
    ElementType eps = 1e-5;
    testing::ProcessTest("Testing BinaryConvolutionalLayer (gemm), values",
                         (testing::IsEqual(output1(0, 0, 0), -20.55556, eps) &&
                          testing::IsEqual(output1(0, 0, 1), -9.66667, eps) &&
                          testing::IsEqual(output1(0, 1, 0), -20.55556, eps) &&
                          testing::IsEqual(output1(0, 1, 1), -9.66667, eps)));

    // Create model
    model::Model model1;
    auto inputNode1 = model1.AddNode<model::InputNode<double>>(input.Size());
    auto computeNode1 = model1.AddNode<nodes::BinaryConvolutionalLayerNode<double>>(inputNode1->output, layer1);

    inputNode1->SetInput(input.ToArray());
    auto modelOutput1 = model1.ComputeOutput(computeNode1->output);
    testing::ProcessTest("Testing BinaryConvolutionalLayer (diagonal) compute", testing::IsEqual(modelOutput1, output1.ToArray()));

    //
    // Verify BinaryConvolutionalLayer with bitwise method. Since we're doing bitwise operations, change the padding scheme to be zeros.
    //
    convolutionalParams.method = BinaryConvolutionMethod::bitwise;
    parameters.inputPaddingParameters.paddingScheme = PaddingScheme::zeros;
    input.Fill(0);
    input(1, 1, 0) = 2;
    input(1, 2, 0) = 1;
    input(1, 1, 1) = 3;
    input(1, 2, 1) = 2;

    BinaryConvolutionalLayer<ElementType> layer2(parameters, convolutionalParams, weights);
    layer2.Compute();
    auto output2 = layer2.GetOutput();

    testing::ProcessTest("Testing BinaryConvolutionalLayer (bitwise), values",
                         (testing::IsEqual(output2(0, 0, 0), 8.22222, eps) &&
                          testing::IsEqual(output2(0, 0, 1), 6.44444, eps) &&
                          testing::IsEqual(output2(0, 1, 0), 8.22222, eps) &&
                          testing::IsEqual(output2(0, 1, 1), 6.44444, eps)));

    // Create model
    model::Model model2;
    auto inputNode2 = model2.AddNode<model::InputNode<double>>(input.Size());
    auto computeNode2 = model2.AddNode<nodes::BinaryConvolutionalLayerNode<double>>(inputNode2->output, layer2);

    inputNode2->SetInput(input.ToArray());
    auto modelOutput2 = model2.ComputeOutput(computeNode2->output);
    testing::ProcessTest("Testing BinaryConvolutionalLayer (bitwise) compute", testing::IsEqual(modelOutput2, output2.ToArray()));
}

static void TestFullyConnectedLayerNode()
{
    using LayerType = predictors::neural::FullyConnectedLayer<double>;

    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using MatrixType = typename LayerType::MatrixType;
    using Shape = typename LayerType::Shape;

    // Set up layer
    TensorType input(2, 2, 2);
    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;
    Shape outputShape = { 4, 1, 1 };
    LayerParameters parameters{ input, predictors::neural::NoPadding(), outputShape, predictors::neural::NoPadding() };

    MatrixType weights(4, 8);
    for (int index = 0; index < 8; index++)
        weights(1, index) = static_cast<double>(index);
    for (int index = 0; index < 8; index++)
        weights(2, index) = static_cast<double>(7 - index);

    predictors::neural::FullyConnectedLayer<double> fullyConnectedLayer(parameters, weights);
    fullyConnectedLayer.Compute();
    auto output = fullyConnectedLayer.GetOutput();

    const double v1 = 0;
    const double v2 = 1 * 0 + 2 * 2 + 3 * 5 + 4 * 7;
    const double v3 = 1 * 7 + 2 * 5 + 3 * 2 + 4 * 0;
    const double v4 = 0;
    testing::ProcessTest("Testing FullyConnectedLayer, values",
                         (testing::IsEqual(output(0, 0, 0), v1) &&
                          testing::IsEqual(output(1, 0, 0), v2) &&
                          testing::IsEqual(output(2, 0, 0), v3) &&
                          testing::IsEqual(output(3, 0, 0), v4)));
    // testing::ProcessTest("Testing FullyConnectedLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(input.Size());
    auto fullyConnectedNode = model.AddNode<nodes::FullyConnectedLayerNode<double>>(inputNode->output, fullyConnectedLayer);

    inputNode->SetInput(input.ToArray());
    auto modelOutput = model.ComputeOutput(fullyConnectedNode->output);
    testing::ProcessTest("Testing FullyConnectedLayerNode compute", testing::IsEqual(modelOutput, output.ToArray()));
}

static void TestPoolingLayerNode()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;
    using ElementType = double;
    using LayerType = PoolingLayer<ElementType, MaxPoolingFunction>;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    // Verify PoolingLayer
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
    LayerType layer(parameters, poolingParams);
    layer.Compute();
    auto output = layer.GetOutput();

    const ElementType v1 = 10;
    const ElementType v2 = 20;
    const ElementType v3 = 30;
    const ElementType v4 = 40;
    const ElementType v5 = 11;
    const ElementType v6 = 21;
    const ElementType v7 = 31;
    const ElementType v8 = 41;

    testing::ProcessTest("Testing PoolingLayer, values",
                         (testing::IsEqual(output(1, 1, 0), v1) &&
                          testing::IsEqual(output(1, 2, 0), v2) &&
                          testing::IsEqual(output(2, 1, 0), v3) &&
                          testing::IsEqual(output(2, 2, 0), v4) &&
                          testing::IsEqual(output(1, 1, 1), v5) &&
                          testing::IsEqual(output(1, 2, 1), v6) &&
                          testing::IsEqual(output(2, 1, 1), v7) &&
                          testing::IsEqual(output(2, 2, 1), v8)));
    testing::ProcessTest("Testing PoolingLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);
}

static void TestScalingLayerNode()
{
    using ElementType = double;
    using LayerType = predictors::neural::ScalingLayer<ElementType>;

    using LayerParameters = typename LayerType::LayerParameters;
    using TensorType = typename LayerType::TensorType;
    using Shape = typename LayerType::Shape;
    using VectorType = typename LayerType::VectorType;

    // Set up scaling layer
    TensorType input(2, 2, 2);
    input(0, 0, 0) = 1;
    input(0, 1, 0) = 2;
    input(1, 0, 1) = 3;
    input(1, 1, 1) = 4;
    Shape outputShape = { 4, 4, 2 };
    LayerParameters parameters{ input, predictors::neural::NoPadding(), outputShape, predictors::neural::ZeroPadding(1) };
    VectorType scale({ 5, 10 });

    LayerType layer(parameters, scale);
    layer.Compute();
    auto output = layer.GetOutput();
    const ElementType v1 = 1 * 5;
    const ElementType v2 = 2 * 5;
    const ElementType v3 = 3 * 10;
    const ElementType v4 = 4 * 10;
    testing::ProcessTest("Testing ScalingLayer, values",
                         (testing::IsEqual(output(1, 1, 0), v1) &&
                          testing::IsEqual(output(1, 2, 0), v2) &&
                          testing::IsEqual(output(2, 1, 1), v3) &&
                          testing::IsEqual(output(2, 2, 1), v4)));
    testing::ProcessTest("Testing ScalingLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 3, 1) == 0 && output(3, 3, 1) == 0);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(input.Size());
    auto scalingNode = model.AddNode<nodes::ScalingLayerNode<ElementType>>(inputNode->output, layer);

    inputNode->SetInput(input.ToArray());
    auto modelOutput = model.ComputeOutput(scalingNode->output);
    testing::ProcessTest("Testing ScalingLayerNode compute", testing::IsEqual(modelOutput, output.ToArray()));
}

static void TestSoftmaxLayerNode()
{
    using ElementType = double;
    using LayerType = predictors::neural::SoftmaxLayer<ElementType>;

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

    LayerType layer(parameters);
    layer.Compute();
    auto output = layer.GetOutput();
    ElementType eps = 1e-6;
    testing::ProcessTest("Testing SoftmaxLayer, values", testing::IsEqual(output(1, 1, 0), 0.0900305733, eps) && testing::IsEqual(output(1, 1, 1), 0.244728476, eps) && testing::IsEqual(output(1, 1, 2), 0.665240943, eps));
    testing::ProcessTest("Testing SoftmaxLayer, padding", output(0, 0, 0) == 0 && output(0, 1, 0) == 0 && output(2, 2, 0) == 0 && output(2, 2, 1) == 0);

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(input.Size());
    auto computeNode = model.AddNode<nodes::SoftmaxLayerNode<double>>(inputNode->output, layer);

    inputNode->SetInput(input.ToArray());
    auto modelOutput = model.ComputeOutput(computeNode->output);
    testing::ProcessTest("Testing SoftmaxLayerNode compute", testing::IsEqual(modelOutput, output.ToArray()));
}

void NeuralNetworkLayerNodesTests()
{
    // Neural nets
    TestNeuralNetworkPredictorNode();
    TestActivationLayerNode();
    TestBatchNormalizationLayerNode();
    TestBiasLayerNode();
    TestBinaryConvolutionalLayerNode();
    TestConvolutionalLayerNode();
    TestFullyConnectedLayerNode();
    TestPoolingLayerNode();
    TestScalingLayerNode();
    TestSoftmaxLayerNode();

    TestArchiveNeuralNetworkPredictorNode();
    TestArchiveNeuralNetworkLayerNodes();
}
