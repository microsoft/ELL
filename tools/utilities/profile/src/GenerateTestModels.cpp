////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GenerateTestModels.cpp (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GenerateTestModels.h"

// math
#include "Tensor.h"

// nodes
#include "BroadcastFunctionNode.h"
#include "ForestPredictorNode.h"
#include "NeuralNetworkPredictorNode.h"

// predictors
#include "ForestPredictor.h"
#include "LinearPredictor.h"
#include "NeuralNetworkPredictor.h"
#include "SingleElementThresholdPredictor.h"

// predictors/neural
#include "ActivationLayer.h"
#include "BatchNormalizationLayer.h"
#include "BinaryConvolutionalLayer.h"
#include "ConvolutionalLayer.h"
#include "FullyConnectedLayer.h"
#include "Layer.h"
#include "MaxPoolingFunction.h"
#include "PoolingLayer.h"
#include "ScalingLayer.h"
#include "SoftmaxLayer.h"
#include "ReLUActivation.h"

// utilities
#include "RandomEngines.h"

namespace ell
{
using namespace predictors::neural;

size_t GetShapeSize(const math::IntegerTriplet& shape)
{
    return shape[0] * shape[1] * shape[2];
}

template <typename ValueType>
class Uniform
{
public:
    Uniform(ValueType minVal, ValueType maxVal, std::string seed = "123")
        : _rng(utilities::GetRandomEngine(seed)), _range(static_cast<double>(_rng.max() - _rng.min())), _minOutput(minVal), _outputRange(maxVal - minVal) {}

    ValueType operator()()
    {
        double uniform = static_cast<double>(_rng()) / _range;
        return static_cast<ValueType>((uniform * _outputRange) + _minOutput);
    }

private:
    std::default_random_engine _rng;
    double _range;
    ValueType _minOutput;
    ValueType _outputRange;
};

template <typename ElementType>
void FillRandomVector(std::vector<ElementType>& vector, ElementType min = -1, ElementType max = 1)
{
    Uniform<ElementType> rand(min, max);
    std::generate(vector.begin(), vector.end(), rand);
}

template <typename ElementType>
void FillRandomVector(ell::math::ColumnVector<ElementType>& vector, ElementType min = -1, ElementType max = 1)
{
    Uniform<ElementType> rand(min, max);
    vector.Generate(rand);
}

template <typename ElementType>
void FillRandomMatrix(ell::math::RowMatrix<ElementType>& matrix, ElementType min = -1, ElementType max = 1)
{
    Uniform<ElementType> rand(min, max);
    matrix.Generate(rand);
}

template <typename ElementType>
void FillRandomTensor(ell::math::ChannelColumnRowTensor<ElementType>& tensor, ElementType min = -1, ElementType max = 1)
{
    Uniform<ElementType> rand(min, max);
    tensor.Generate(rand);
}

//
//
//

predictors::SimpleForestPredictor CreateForest(size_t numSplits)
{
    // define some abbreviations
    using SplitAction = predictors::SimpleForestPredictor::SplitAction;
    using SplitRule = predictors::SingleElementThresholdPredictor;
    using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;

    // build a forest
    predictors::SimpleForestPredictor forest;
    SplitRule dummyRule{ 0, 0.0 };
    EdgePredictorVector dummyEdgePredictor{ -1.0, 1.0 };
    auto root = forest.Split(SplitAction{ forest.GetNewRootId(), dummyRule, dummyEdgePredictor });
    std::vector<size_t> interiorNodeVector;
    interiorNodeVector.push_back(root);

    for (size_t index = 0; index < numSplits; ++index)
    {
        auto node = interiorNodeVector.back();
        interiorNodeVector.pop_back();
        interiorNodeVector.push_back(forest.Split(SplitAction{ forest.GetChildId(node, 0), dummyRule, dummyEdgePredictor }));
        interiorNodeVector.push_back(forest.Split(SplitAction{ forest.GetChildId(node, 1), dummyRule, dummyEdgePredictor }));
    }
    return forest;
}

model::Map GenerateTreeModel(size_t numSplits)
{
    auto forest = CreateForest(numSplits);
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto computeNode = model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", computeNode->output } });
    return map;
}

//
// Neural nets
//

model::Map GenerateBinaryConvolutionModel(size_t imageRows, size_t imageColumns, size_t numChannels, size_t numFilters)
{
    using namespace predictors::neural;

    using ElementType = float;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    const size_t k = 3;
    const size_t stride = 1;
    const size_t inputPaddingSize = 1;
    const size_t outputPaddingSize = 0;
    const auto paddingScheme = PaddingScheme::zeros;
    const bool scaleByFilterMeans = true;

    typename predictors::NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename predictors::NeuralNetworkPredictor<ElementType>::Layers layers;

    Shape inputShape = { imageRows, imageColumns, numChannels };
    Shape paddedInputShape = { imageRows + 2 * inputPaddingSize, imageColumns + 2 * inputPaddingSize, numChannels };
    Shape outputShape = { imageRows + 2 * outputPaddingSize, imageColumns + 2 * outputPaddingSize, numFilters };

    // Input layer
    InputParameters inputParams = { inputShape, NoPadding(), paddedInputShape, { paddingScheme, inputPaddingSize }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    LayerParameters layerParams{ inputLayer->GetOutput(), { paddingScheme, inputPaddingSize }, outputShape, { paddingScheme, outputPaddingSize } };
    BinaryConvolutionalParameters convolutionalParams{ k, stride, BinaryConvolutionMethod::bitwise, scaleByFilterMeans ? BinaryWeightsScale::mean : BinaryWeightsScale::none };
    TensorType convWeights(convolutionalParams.receptiveField * outputShape.NumChannels(), convolutionalParams.receptiveField, numChannels);
    FillRandomTensor(convWeights);

    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BinaryConvolutionalLayer<ElementType>(layerParams, convolutionalParams, convWeights)));

    predictors::NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, neuralNetwork);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", predictorNode->output } });
    return map;
}

model::Map GenerateBinaryConvolutionPlusDenseModel(size_t imageRows, size_t imageColumns, size_t numChannels, size_t numFilters, size_t numOutputs)
{
    using ElementType = float;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using LayerParameters = typename Layer<ElementType>::LayerParameters;
    using MatrixType = typename Layer<ElementType>::MatrixType;
    using TensorType = typename Layer<ElementType>::TensorType;
    using Shape = typename Layer<ElementType>::Shape;

    const size_t inputPaddingSize = 1;
    const size_t outputPaddingSize = 0;
    const auto paddingScheme = PaddingScheme::zeros;
    const bool scaleByFilterMeans = true;

    typename predictors::NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename predictors::NeuralNetworkPredictor<ElementType>::Layers layers;

    Shape inputShape = { imageRows, imageColumns, numChannels };
    Shape paddedInputShape = { imageRows + 2 * inputPaddingSize, imageColumns + 2 * inputPaddingSize, numChannels };
    Shape outputShape = { imageRows + 2 * outputPaddingSize, imageColumns + 2 * outputPaddingSize, numFilters };

    // Input layer
    InputParameters inputParams = { inputShape, NoPadding(), paddedInputShape, { paddingScheme, inputPaddingSize }, 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    // Binary convolutional layer
    LayerParameters layerParams{ inputLayer->GetOutput(), { paddingScheme, inputPaddingSize }, outputShape, { paddingScheme, outputPaddingSize } };
    BinaryConvolutionalParameters convolutionalParams{ 3, 1, BinaryConvolutionMethod::bitwise, scaleByFilterMeans ? BinaryWeightsScale::mean : BinaryWeightsScale::none };
    TensorType convWeights(convolutionalParams.receptiveField * outputShape.NumChannels(), convolutionalParams.receptiveField, numChannels);
    FillRandomTensor(convWeights);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new BinaryConvolutionalLayer<ElementType>(layerParams, convolutionalParams, convWeights)));

    // Dense layer
    layerParams = { layers.back()->GetOutput(), NoPadding(), { 1, 1, numOutputs }, NoPadding() };
    MatrixType denseWeights(numOutputs, imageRows * imageColumns * numFilters);
    FillRandomMatrix(denseWeights);
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new FullyConnectedLayer<ElementType>(layerParams, denseWeights)));

    // Create predictor
    predictors::NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, neuralNetwork);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", predictorNode->output } });
    return map;
}

template <typename LayerType, typename ElementType, typename... Args>
void AddLayer(typename predictors::NeuralNetworkPredictor<ElementType>::Layers& layers, typename predictors::NeuralNetworkPredictor<ElementType>::InputLayerReference& inputLayer, const PaddingParameters& inputPadding, const typename Layer<ElementType>::Shape& outputShape, const PaddingParameters& outputPadding, Args... args)
{
    assert(layers.size() == 0);
    typename Layer<ElementType>::LayerParameters layerParams = { inputLayer->GetOutput(), inputPadding, outputShape, outputPadding };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new LayerType(layerParams, args...)));
}

template <typename LayerType, typename ElementType, typename... Args>
void AddLayer(typename predictors::NeuralNetworkPredictor<ElementType>::Layers& layers, const PaddingParameters& inputPadding, const typename Layer<ElementType>::Shape& outputShape, const PaddingParameters& outputPadding, Args... args)
{
    typename Layer<ElementType>::LayerParameters layerParams = { layers.back()->GetOutput(), inputPadding, outputShape, outputPadding };
    layers.push_back(std::unique_ptr<Layer<ElementType>>(new LayerType(layerParams, args...)));
}

template <typename ElementType>
typename Layer<ElementType>::Shape PadShape(const typename Layer<ElementType>::Shape& inputShape, size_t padding)
{
    return { inputShape[0] + 2 * padding, inputShape[1] + 2 * padding, inputShape[2] };
}

template <typename VectorType>
VectorType GetRandomVector(size_t size)
{
    VectorType vector(size);
    FillRandomVector(vector);
    return vector;
}

template <typename MatrixType>
MatrixType GetRandomMatrix(size_t rows, size_t columns)
{
    MatrixType matrix(rows, columns);
    FillRandomMatrix(matrix);
    return matrix;
}

template <typename TensorType>
TensorType GetRandomTensor(size_t rows, size_t columns, size_t channels)
{
    TensorType tensor(rows, columns, channels);
    FillRandomTensor(tensor);
    return tensor;
}

model::Map GenerateBinaryDarknetLikeModel(bool lastLayerReal)
{
    using ElementType = float;
    using InputParameters = typename InputLayer<ElementType>::InputParameters;
    using VectorType = typename Layer<ElementType>::VectorType;
    using TensorType = typename Layer<ElementType>::TensorType;

    const bool scaleByFilterMeans = true;

    typename predictors::NeuralNetworkPredictor<ElementType>::InputLayerReference inputLayer;
    typename predictors::NeuralNetworkPredictor<ElementType>::Layers layers;

    ElementType eps = static_cast<ElementType>(1e-6);
    auto epsVar = EpsilonSummand::Variance;

    // Variables for weights, etc:
    VectorType bias;
    VectorType scale;
    VectorType bnMean;
    VectorType bnVar;
    BinaryConvolutionalParameters convParams{ 3, 1, BinaryConvolutionMethod::bitwise, scaleByFilterMeans ? BinaryWeightsScale::mean : BinaryWeightsScale::none };
    ConvolutionalParameters realConvParams{ 3, 1, ConvolutionMethod::unrolled, 1 };

    // Input layer:  160x160x3
    InputParameters inputParams = { {160, 160, 3}, NoPadding(), {160, 160, 3}, NoPadding(), 1 };
    inputLayer = std::make_unique<InputLayer<ElementType>>(inputParams);

    // BiasLayer<float>(shape=[160,160,3]->[162,162,3], outputPadding=zeros,1)
    bias = GetRandomVector<VectorType>(3);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, inputLayer, NoPadding(), {162, 162, 3}, ZeroPadding(1), bias);

    // BinaryConvolutionalLayer<float>(shape=[162,162,3]->[160,160,16], inputPadding=zeros,1, stride=1, method=unrolled, receptiveField=3, numFilters=16)
    auto convWeights = GetRandomTensor<TensorType>(3 * 16, 3, 3); // k * f, k, ch
    AddLayer<BinaryConvolutionalLayer<ElementType>, ElementType>(layers, ZeroPadding(1), {160, 160, 16}, NoPadding(), convParams, convWeights);

    // BiasLayer<float>(shape=[160,160,16]->[160,160,16])
    bias = GetRandomVector<VectorType>(16);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {160, 160, 16}, NoPadding(), bias);

    // ActivationLayer<float,ReLUActivation>(shape=[160,160,16]->[160,160,16])
    AddLayer<ActivationLayer<ElementType, ReLUActivation>, ElementType>(layers, NoPadding(), {160, 160, 16}, NoPadding());

    // BatchNormalizationLayer<float>(shape=[160,160,16]->[160,160,16])
    bnMean = GetRandomVector<VectorType>(16);
    bnVar = GetRandomVector<VectorType>(16);
    AddLayer<BatchNormalizationLayer<ElementType>, ElementType>(layers, NoPadding(), {160, 160, 16}, NoPadding(), bnMean, bnVar, eps, epsVar);

    // ScalingLayer<float>(shape=[160,160,16]->[160,160,16])
    scale = GetRandomVector<VectorType>(16);
    AddLayer<ScalingLayer<ElementType>, ElementType>(layers, NoPadding(), {160, 160, 16}, NoPadding(), scale);

    // BiasLayer<float>(shape=[160,160,16]->[160,160,16])
    bias = GetRandomVector<VectorType>(16);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {160, 160, 16}, NoPadding(), bias);

    // PoolingLayer<float,MaxPoolingFunction>(shape=[160,160,16]->[82,82,16], outputPadding=zeros,1, function=maxpooling, stride=2, size=2)
    AddLayer<PoolingLayer<ElementType, MaxPoolingFunction>, ElementType>(layers, NoPadding(), {82, 82, 16}, ZeroPadding(1), PoolingParameters{2, 2});

    // BinaryConvolutionalLayer<float>(shape=[82,82,16]->[80,80,64], inputPadding=zeros,1, stride=1, method=unrolled, receptiveField=3, numFilters=64)
    convWeights = GetRandomTensor<TensorType>(3 * 64, 3, 16); // k * f, k, ch
    AddLayer<BinaryConvolutionalLayer<ElementType>, ElementType>(layers, ZeroPadding(1), {80, 80, 64}, NoPadding(), convParams, convWeights);

    // BiasLayer<float>(shape=[80,80,64]->[80,80,64])
    bias = GetRandomVector<VectorType>(64);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {80, 80, 64}, NoPadding(), bias);

    // ActivationLayer<float,ReLUActivation>(shape=[80,80,64]->[80,80,64])
    AddLayer<ActivationLayer<ElementType, ReLUActivation>, ElementType>(layers, NoPadding(), {80, 80, 64}, NoPadding());

    // BatchNormalizationLayer<float>(shape=[80,80,64]->[80,80,64])
    bnMean = GetRandomVector<VectorType>(64);
    bnVar = GetRandomVector<VectorType>(64);
    AddLayer<BatchNormalizationLayer<ElementType>, ElementType>(layers, NoPadding(), {80, 80, 64}, NoPadding(), bnMean, bnVar, eps, epsVar);

    // ScalingLayer<float>(shape=[80,80,64]->[80,80,64])
    scale = GetRandomVector<VectorType>(64);
    AddLayer<ScalingLayer<ElementType>, ElementType>(layers, NoPadding(), {80, 80, 64}, NoPadding(), scale);

    // BiasLayer<float>(shape=[80,80,64]->[80,80,64])
    bias = GetRandomVector<VectorType>(64);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {80, 80, 64}, NoPadding(), bias);

    // PoolingLayer<float,MaxPoolingFunction>(shape=[80,80,64]->[42,42,64], outputPadding=zeros,1, function=maxpooling, stride=2, size=2)
    AddLayer<PoolingLayer<ElementType, MaxPoolingFunction>, ElementType>(layers, NoPadding(), {42, 42, 64}, ZeroPadding(1), PoolingParameters{2, 2});

    // BinaryConvolutionalLayer<float>(shape=[42,42,64]->[40,40,64], inputPadding=zeros,1, stride=1, method=bitwise, receptiveField=3, weightsScale=none)
    convWeights = GetRandomTensor<TensorType>(3 * 64, 3, 64); // k * f, k, ch
    AddLayer<BinaryConvolutionalLayer<ElementType>, ElementType>(layers, ZeroPadding(1), {40, 40, 64}, NoPadding(), convParams, convWeights);

    // BiasLayer<float>(shape=[40,40,64]->[40,40,64])
    bias = GetRandomVector<VectorType>(64);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {40, 40, 64}, NoPadding(), bias);

    // ActivationLayer<float,ParametricReLUActivation>(shape=[40,40,64]->[42,42,64], outputPadding=min,1)
    AddLayer<ActivationLayer<ElementType, ReLUActivation>, ElementType>(layers, NoPadding(), {42, 42, 64}, ZeroPadding(1));

    // PoolingLayer<float,MaxPoolingFunction>(shape=[42,42,64]->[20,20,64], inputPadding=min,1, function=maxpooling, stride=2, size=3)
    AddLayer<PoolingLayer<ElementType, MaxPoolingFunction>, ElementType>(layers, ZeroPadding(1), {20, 20, 64}, NoPadding(), PoolingParameters{3, 2});

    // BatchNormalizationLayer<float>(shape=[20,20,64]->[20,20,64])
    bnMean = GetRandomVector<VectorType>(64);
    bnVar = GetRandomVector<VectorType>(64);
    AddLayer<BatchNormalizationLayer<ElementType>, ElementType>(layers, NoPadding(), {20, 20, 64}, NoPadding(), bnMean, bnVar, eps, epsVar);

    // ScalingLayer<float>(shape=[20,20,64]->[20,20,64])
    scale = GetRandomVector<VectorType>(64);
    AddLayer<ScalingLayer<ElementType>, ElementType>(layers, NoPadding(), {20, 20, 64}, NoPadding(), scale);

    // BiasLayer<float>(shape=[20,20,64]->[22,22,64], outputPadding=zeros,1)
    bias = GetRandomVector<VectorType>(64);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {22, 22, 64}, ZeroPadding(1), bias);

    // BinaryConvolutionalLayer<float>(shape=[22,22,64]->[20,20,128], inputPadding=zeros,1, stride=1, method=bitwise, receptiveField=3, weightsScale=none)
    convWeights = GetRandomTensor<TensorType>(3 * 128, 3, 64); // k * f, k, ch
    AddLayer<BinaryConvolutionalLayer<ElementType>, ElementType>(layers, ZeroPadding(1), {20, 20, 128}, NoPadding(), convParams, convWeights);

    // BiasLayer<float>(shape=[20,20,128]->[20,20,128])
    bias = GetRandomVector<VectorType>(128);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {20, 20, 128}, NoPadding(), bias);

    // ActivationLayer<float,ParametricReLUActivation>(shape=[20,20,128]->[22,22,128], outputPadding=min,1)
    AddLayer<ActivationLayer<ElementType, ReLUActivation>, ElementType>(layers, NoPadding(), {22, 22, 128}, ZeroPadding(1));

    // PoolingLayer<float,MaxPoolingFunction>(shape=[22,22,128]->[10,10,128], inputPadding=min,1, function=maxpooling, stride=2, size=3)
    AddLayer<PoolingLayer<ElementType, MaxPoolingFunction>, ElementType>(layers, ZeroPadding(1), {10, 10, 128}, NoPadding(), PoolingParameters{3, 2});

    // BatchNormalizationLayer<float>(shape=[10,10,128]->[10,10,128])
    bnMean = GetRandomVector<VectorType>(128);
    bnVar = GetRandomVector<VectorType>(128);
    AddLayer<BatchNormalizationLayer<ElementType>, ElementType>(layers, NoPadding(), {10, 10, 128}, NoPadding(), bnMean, bnVar, eps, epsVar);

    // ScalingLayer<float>(shape=[10,10,128]->[10,10,128])
    scale = GetRandomVector<VectorType>(128);
    AddLayer<ScalingLayer<ElementType>, ElementType>(layers, NoPadding(), {10, 10, 128}, NoPadding(), scale);

    // BiasLayer<float>(shape=[10,10,128]->[12,12,128], outputPadding=zeros,1)
    bias = GetRandomVector<VectorType>(128);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {12, 12, 128}, ZeroPadding(1), bias);

    // BinaryConvolutionalLayer<float>(shape=[12,12,128]->[10,10,256], inputPadding=zeros,1, stride=1, method=bitwise, receptiveField=3, weightsScale=none)
    convWeights = GetRandomTensor<TensorType>(3 * 256, 3, 128); // k * f, k, ch
    AddLayer<BinaryConvolutionalLayer<ElementType>, ElementType>(layers, ZeroPadding(1), {10, 10, 256}, NoPadding(), convParams, convWeights);

    // BiasLayer<float>(shape=[10,10,256]->[10,10,256])
    bias = GetRandomVector<VectorType>(256);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {10, 10, 256}, NoPadding(), bias);

    // ActivationLayer<float,ParametricReLUActivation>(shape=[10,10,256]->[12,12,256], outputPadding=min,1)
    AddLayer<ActivationLayer<ElementType, ReLUActivation>, ElementType>(layers, NoPadding(), {12, 12, 256}, ZeroPadding(1));

    // PoolingLayer<float,MaxPoolingFunction>(shape=[12,12,256]->[5,5,256], inputPadding=min,1, function=maxpooling, stride=2, size=3)
    AddLayer<PoolingLayer<ElementType, MaxPoolingFunction>, ElementType>(layers, ZeroPadding(1), {5, 5, 256}, NoPadding(), PoolingParameters{3, 2});

    // BatchNormalizationLayer<float>(shape=[5,5,256]->[5,5,256])
    bnMean = GetRandomVector<VectorType>(256);
    bnVar = GetRandomVector<VectorType>(256);
    AddLayer<BatchNormalizationLayer<ElementType>, ElementType>(layers, NoPadding(), {5, 5, 256}, NoPadding(), bnMean, bnVar, eps, epsVar);

    // ScalingLayer<float>(shape=[5,5,256]->[5,5,256])
    scale = GetRandomVector<VectorType>(256);
    AddLayer<ScalingLayer<ElementType>, ElementType>(layers, NoPadding(), {5, 5, 256}, NoPadding(), scale);

    // BiasLayer<float>(shape=[5,5,256]->[7,7,256], outputPadding=zeros,1)
    bias = GetRandomVector<VectorType>(256);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {7, 7, 256}, ZeroPadding(1), bias);

    // BinaryConvolutionalLayer<float>(shape=[7,7,256]->[5,5,512], inputPadding=zeros,1, stride=1, method=bitwise, receptiveField=3, weightsScale=none)
    convWeights = GetRandomTensor<TensorType>(3 * 512, 3, 256); // k * f, k, ch
    AddLayer<BinaryConvolutionalLayer<ElementType>, ElementType>(layers, ZeroPadding(1), {5, 5, 512}, NoPadding(), convParams, convWeights);

    // BiasLayer<float>(shape=[5,5,512]->[5,5,512])
    bias = GetRandomVector<VectorType>(512);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {5, 5, 512}, NoPadding(), bias);

    // ActivationLayer<float,ParametricReLUActivation>(shape=[5,5,512]->[7,7,512], outputPadding=min,1)
    AddLayer<ActivationLayer<ElementType, ReLUActivation>, ElementType>(layers, NoPadding(), {7, 7, 512}, ZeroPadding(1));

    // PoolingLayer<float,MaxPoolingFunction>(shape=[7,7,512]->[3,3,512], inputPadding=min,1, function=maxpooling, stride=2, size=3)
    AddLayer<PoolingLayer<ElementType, MaxPoolingFunction>, ElementType>(layers, ZeroPadding(1), {3, 3, 512}, NoPadding(), PoolingParameters{3, 2});

    // BatchNormalizationLayer<float>(shape=[3,3,512]->[3,3,512])
    bnMean = GetRandomVector<VectorType>(512);
    bnVar = GetRandomVector<VectorType>(512);
    AddLayer<BatchNormalizationLayer<ElementType>, ElementType>(layers, NoPadding(), {3, 3, 512}, NoPadding(), bnMean, bnVar, eps, epsVar);

    // ScalingLayer<float>(shape=[3,3,512]->[3,3,512])
    scale = GetRandomVector<VectorType>(512);
    AddLayer<ScalingLayer<ElementType>, ElementType>(layers, NoPadding(), {3, 3, 512}, NoPadding(), scale);

    // BiasLayer<float>(shape=[3,3,512]->[5,5,512], outputPadding=zeros,1)
    bias = GetRandomVector<VectorType>(512);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {5, 5, 512}, ZeroPadding(1), bias);

    // BinaryConvolutionalLayer<float>(shape=[5,5,512]->[3,3,1024], inputPadding=zeros,1, stride=1, method=bitwise, receptiveField=3, weightsScale=none)
    convWeights = GetRandomTensor<TensorType>(3 * 1024, 3, 512); // k * f, k, ch
    AddLayer<BinaryConvolutionalLayer<ElementType>, ElementType>(layers, ZeroPadding(1), {3, 3, 1024}, NoPadding(), convParams, convWeights);

    // BiasLayer<float>(shape=[3,3,1024]->[3,3,1024])
    bias = GetRandomVector<VectorType>(1024);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {3, 3, 1024}, NoPadding(), bias);

    // ActivationLayer<float,ParametricReLUActivation>(shape=[3,3,1024]->[5,5,1024], outputPadding=min,1)
    AddLayer<ActivationLayer<ElementType, ReLUActivation>, ElementType>(layers, NoPadding(), {5, 5, 1024}, ZeroPadding(1));

    // PoolingLayer<float,MaxPoolingFunction>(shape=[5,5,1024]->[2,2,1024], inputPadding=min,1, function=maxpooling, stride=2, size=3)
    AddLayer<PoolingLayer<ElementType, MaxPoolingFunction>, ElementType>(layers, ZeroPadding(1), {2, 2, 1024}, NoPadding(), PoolingParameters{3, 2});

    // BatchNormalizationLayer<float>(shape=[2,2,1024]->[2,2,1024])
    bnMean = GetRandomVector<VectorType>(1024);
    bnVar = GetRandomVector<VectorType>(1024);
    AddLayer<BatchNormalizationLayer<ElementType>, ElementType>(layers, NoPadding(), {2, 2, 1024}, NoPadding(), bnMean, bnVar, eps, epsVar);

    // ScalingLayer<float>(shape=[2,2,1024]->[2,2,1024])
    scale = GetRandomVector<VectorType>(1024);
    AddLayer<ScalingLayer<ElementType>, ElementType>(layers, NoPadding(), {2, 2, 1024}, NoPadding(), scale);

    // BiasLayer<float>(shape=[2,2,1024]->[2,2,1024])
    bias = GetRandomVector<VectorType>(1024);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {2, 2, 1024}, NoPadding(), bias);

    // ConvolutionalLayer<float>(shape=[2,2,1024]->[2,2,1000], stride=1, method=unrolled, receptiveField=1, numFilters=1000)
    convWeights = GetRandomTensor<TensorType>(3 * 1000, 3, 1024); // k * f, k, ch
    if(lastLayerReal)
    {
        AddLayer<ConvolutionalLayer<ElementType>, ElementType>(layers, NoPadding(), {2, 2, 1000}, NoPadding(), realConvParams, convWeights);
    }
    else
    {
        AddLayer<BinaryConvolutionalLayer<ElementType>, ElementType>(layers, NoPadding(), {2, 2, 1000}, NoPadding(), convParams, convWeights);
    }

    // BiasLayer<float>(shape=[2,2,1000]->[2,2,1000])
    bias = GetRandomVector<VectorType>(1000);
    AddLayer<BiasLayer<ElementType>, ElementType>(layers, NoPadding(), {2, 2, 1000}, NoPadding(), bias);

    // PoolingLayer<float,MeanPoolingFunction>(shape=[2,2,1000]->[1,1,1000], function=meanpooling, stride=1, size=2)
    AddLayer<PoolingLayer<ElementType, MaxPoolingFunction>, ElementType>(layers, NoPadding(), {1, 1, 1000}, NoPadding(), PoolingParameters{2, 1});

    // ScalingLayer<float>(shape=[1,1,1000]->[1,1,1000])
    scale = GetRandomVector<VectorType>(1000);
    AddLayer<ScalingLayer<ElementType>, ElementType>(layers, NoPadding(), {1, 1, 1000}, NoPadding(), scale);

    // SoftmaxLayer<float>(shape=[1,1,1000]->[1,1,1000])
    AddLayer<SoftmaxLayer<ElementType>, ElementType>(layers, NoPadding(), {1, 1, 1000}, NoPadding());

    //
    // Create predictor
    //
    predictors::NeuralNetworkPredictor<ElementType> neuralNetwork(std::move(inputLayer), std::move(layers));

    // Create model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(GetShapeSize(neuralNetwork.GetInputShape()));
    auto predictorNode = model.AddNode<nodes::NeuralNetworkPredictorNode<ElementType>>(inputNode->output, neuralNetwork);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", predictorNode->output } });
    return map;
}
}
