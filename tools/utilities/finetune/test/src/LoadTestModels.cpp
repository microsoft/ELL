////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadTestModels.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadTestModels.h"

#include <common/include/LoadModel.h>

#include <math/include/Tensor.h>

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>
#include <model/include/OutputPort.h>

#include <nodes/include/ConvolutionalLayerNode.h>
#include <nodes/include/FullyConnectedLayerNode.h>

#include <predictors/neural/include/ConvolutionalLayer.h>
#include <predictors/neural/include/FullyConnectedLayer.h>
#include <predictors/neural/include/Layer.h>

#include <testing/include/testing.h>

#include <utilities/include/Exception.h>
#include <utilities/include/Files.h>
#include <utilities/include/MemoryLayout.h>

#include <iostream>

using namespace ell;
using namespace ell::model;

namespace
{
math::TensorShape ToTensorShape(const utilities::MemoryShape& shape)
{
    auto tensorShape = shape;
    tensorShape.Resize(3);
    return math::TensorShape(tensorShape[0], tensorShape[1], tensorShape[2]);
}

predictors::neural::FullyConnectedLayer<float> CreateFullyConnectedLayer(int inputSize, int outputSize)
{
    typename predictors::neural::Layer<float>::TensorType inputTensorPlaceholder(math::TensorShape{ static_cast<size_t>(inputSize), 1, 1 });
    typename predictors::neural::Layer<float>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<float>::LayerParameters params{ inputTensorPlaceholderRef, predictors::neural::NoPadding(), { static_cast<size_t>(outputSize), 1, 1 }, predictors::neural::NoPadding() };
    typename predictors::neural::FullyConnectedLayer<float>::MatrixType weights(outputSize, inputSize);
    return { params, weights };
}

const OutputPort<float>& AppendFullyConnectedNode(Model& model, const model::OutputPort<float>& input, int outputSize)
{
    auto newLayer = CreateFullyConnectedLayer(input.Size(), outputSize);
    auto newNode = model.AddNode<nodes::FullyConnectedLayerNode<float>>(input, newLayer);
    return newNode->output;
}

predictors::neural::ConvolutionalLayer<float> CreateConvolutionalLayer(const utilities::MemoryShape& inputShape, int filterSize, int numFilters)
{
    const auto numRows = inputShape[0];
    const auto numColumns = inputShape[1];
    const auto numChannels = inputShape[2];

    const int stride = 1;
    const auto numOutputRows = (numRows - filterSize + 1) / stride;
    const auto numOutputColumns = (numColumns - filterSize + 1) / stride;
    const auto numOutputChannels = numFilters;

    typename predictors::neural::Layer<float>::TensorType inputTensorPlaceholder(ToTensorShape(inputShape));
    typename predictors::neural::Layer<float>::ConstTensorReferenceType inputTensorPlaceholderRef(inputTensorPlaceholder);
    typename predictors::neural::Layer<float>::LayerParameters layerParams{ inputTensorPlaceholderRef, predictors::neural::NoPadding(), { static_cast<size_t>(numOutputRows), static_cast<size_t>(numOutputColumns), static_cast<size_t>(numOutputChannels) }, predictors::neural::NoPadding() };
    predictors::neural::ConvolutionalParameters convParams{ static_cast<size_t>(filterSize), static_cast<size_t>(stride), predictors::neural::ConvolutionMethod::automatic, 1 };
    math::ChannelColumnRowTensor<float> weightsTensor(static_cast<size_t>(numOutputChannels * filterSize), static_cast<size_t>(filterSize), numChannels);
    return { layerParams, convParams, weightsTensor };
}

const OutputPort<float>& AppendConvolutionalNode(Model& model, const model::OutputPort<float>& input, int filterSize, int numFilters)
{
    auto newLayer = CreateConvolutionalLayer(input.GetMemoryLayout().GetActiveSize(), filterSize, numFilters);
    auto newNode = model.AddNode<nodes::ConvolutionalLayerNode<float>>(input, newLayer);
    return newNode->output;
}
} // namespace

std::string GetFullyConnectedMNISTModelPath()
{
    throw testing::TestNotImplementedException("GetFullyConnectedMNISTModelPath");
}

std::string GetConvolutionalMNISTModelPath()
{
    throw testing::TestNotImplementedException("GetConvolutionalMNISTModelPath");
}

std::string GetSuperSimpleConvolutionalMNISTModelPath()
{
    throw testing::TestNotImplementedException("GetSuperSimpleConvolutionalMNISTModelPath");
}

std::string GetTrivialConvolutionalModelPath()
{
    throw testing::TestNotImplementedException("GetTrivialConvolutionalModelPath");
}

std::string GetTrivialConvolutionalModel2Path()
{
    throw testing::TestNotImplementedException("GetTrivialConvolutionalModel2Path");
}

Model LoadFullyConnectedMNISTModel()
{
    auto modelPath = GetFullyConnectedMNISTModelPath();
    if (!utilities::IsFileReadable(modelPath))
    {
        throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound, "Model file not readable");
    }
    auto model = common::LoadModel(modelPath);
    return model;
}

Model LoadConvolutionalMNISTModel()
{
    auto modelPath = GetConvolutionalMNISTModelPath();
    if (!utilities::IsFileReadable(modelPath))
    {
        throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound, "Model file not readable");
    }
    auto model = common::LoadModel(modelPath);
    model.Print(std::cout);
    return model;
}

Model LoadSuperSimpleConvolutionalMNISTModel()
{
    auto modelPath = GetSuperSimpleConvolutionalMNISTModelPath();
    if (!utilities::IsFileReadable(modelPath))
    {
        throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound, "Model file not readable");
    }
    auto model = common::LoadModel(modelPath);
    model.Print(std::cout);
    return model;
}

Model LoadTrivialConvolutionalModel()
{
    auto modelPath = GetTrivialConvolutionalModelPath();
    if (!utilities::IsFileReadable(modelPath))
    {
        throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound, "Model file not readable");
    }
    auto model = common::LoadModel(modelPath);
    model.Print(std::cout);
    return model;
}

Model LoadTrivialConvolutionalModel2()
{
    auto modelPath = GetTrivialConvolutionalModel2Path();
    if (!utilities::IsFileReadable(modelPath))
    {
        throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound, "Model file not readable");
    }
    auto model = common::LoadModel(modelPath);
    model.Print(std::cout);
    return model;
}

Model GetNodeFindingTestModel()
{
    Model model;
    const utilities::MemoryShape inputShape{ 8, 6, 2 };
    const int outputSize = 10;
    const int filterSize = 3;
    const int numFilters = 4;

    const auto& in = model::Input<float>(model, inputShape);
    const auto& conv = AppendConvolutionalNode(model, in, filterSize, numFilters);
    const auto& fc = AppendFullyConnectedNode(model, conv, outputSize);
    model::Output(fc);
    return model;
}
