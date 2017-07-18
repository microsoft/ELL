////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorInterface.tcc (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// interfaces
#include "NeuralNetworkPredictorInterface.h"

// neural predictor
#include "LeakyReLUActivation.h"
#include "MaxPoolingFunction.h"
#include "MeanPoolingFunction.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"

// utilities
#include "JsonArchiver.h"
#include "Files.h"

// stl
#include <ostream>

namespace ell
{
namespace api
{
namespace predictors
{

    namespace underlying = ell::predictors::neural;
    namespace api = ell::api::predictors::neural;
    //////////////////////////////////////////////////////////////////////////
    // Api classes for the neural predictor
    //////////////////////////////////////////////////////////////////////////
    template <typename ElementType>
    NeuralNetworkPredictor<ElementType>::NeuralNetworkPredictor(const std::vector<ell::api::predictors::neural::Layer<ElementType>*>& layers)
    {
        if (layers.size() > 0)
        {
            auto& parameters = layers.front()->parameters;
            // Construct the input layer
            UnderlyingInputParameters inputParameters =
                {
                  { parameters.inputShape.rows - (2 * parameters.inputPaddingParameters.paddingSize), parameters.inputShape.columns - (2 * parameters.inputPaddingParameters.paddingSize), parameters.inputShape.channels },
                  underlying::NoPadding(),
                  { parameters.inputShape.rows, parameters.inputShape.columns, parameters.inputShape.channels },
                  parameters.inputPaddingParameters,
                  1.0
                };
            auto inputLayer = std::make_unique<underlying::InputLayer<ElementType>>(inputParameters);

            UnderlyingLayers underlyingLayers;

            // Construct up the layers
            for (size_t i = 0; i < layers.size(); i++)
            {
                AddLayer(layers[i], inputLayer, underlyingLayers);
            }

            // Create the predictor
            _predictor = std::make_shared<UnderlyingPredictor>(std::move(inputLayer), std::move(underlyingLayers));
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "'layers' parameters does not contain any neural network layers");
        }
    }

    template <typename ElementType>
    std::vector<ElementType> NeuralNetworkPredictor<ElementType>::Predict(const std::vector<double>& input)
    {
        std::vector<ElementType> result;
        // Call the underlying predictor with the specified input.
        // Note that we return a copy and not a reference, since we cannot control what the
        // api caller is going to do with it.
        if (_predictor != nullptr)
        {
            using UnderlyingDataVectorType = typename UnderlyingPredictor::DataVectorType;
            UnderlyingDataVectorType dataInput(input);
            result = _predictor->Predict(dataInput);
        }
        else
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Predictor must be initialized before Predict method is called");
        }
        return result;
    }

    template <typename ElementType>
    neural::LayerShape NeuralNetworkPredictor<ElementType>::GetInputShape() const
    {
        auto shape = _predictor->GetInputShape();
        return neural::LayerShape{ shape[0], shape[1], shape[2] };
    }

    template <typename ElementType>
    neural::LayerShape NeuralNetworkPredictor<ElementType>::GetOutputShape() const
    {
        auto shape = _predictor->GetOutputShape();
        return neural::LayerShape{ shape[0], shape[1], shape[2] };
    }

    template <typename ElementType>
    template <typename DerivedLayer>
    auto& NeuralNetworkPredictor<ElementType>::LayerAs(Layer* layer)
    {
        if (layer == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Trying to cast null layer");
        }
        if (!layer->template Is<DerivedLayer>())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Bad layer type cast");
        }
        return layer->template As<DerivedLayer>();
    }

    template <typename ElementType>
    void NeuralNetworkPredictor<ElementType>::AddLayer(Layer* layer, const std::unique_ptr<UnderlyingInputLayer>& underlyingInputLayer, UnderlyingLayers& underlyingLayers)
    {
        using UnderlyingLayer = typename ell::predictors::neural::Layer<ElementType>;
        using UnderlyingLayerParameters = typename ell::predictors::neural::Layer<ElementType>::LayerParameters;
        using TensorType = typename underlying::Layer<ElementType>::TensorType;
        if (layer != nullptr)
        {
            // Set the layer parameters. Note that if this is the first layer, we set the input reference to the output of the InputLayer.
            // Otherwise, we set it to the output of the last layer.
            UnderlyingLayerParameters parameters =
                {
                  ((underlyingLayers.size() > 0) ? underlyingLayers.back()->GetOutput() : underlyingInputLayer->GetOutput()),
                  layer->parameters.inputPaddingParameters,
                  { layer->parameters.outputShape.rows, layer->parameters.outputShape.columns, layer->parameters.outputShape.channels },
                  layer->parameters.outputPaddingParameters,
                };

            // Instantiate the specific layer type
            underlying::LayerType layerType = layer->GetLayerType();
            switch (layerType)
            {
                case (underlying::LayerType::activation):
                {
                    auto& apiLayer = LayerAs<api::ActivationLayer<ElementType>>(layer);
                    if (apiLayer.activation == api::ActivationType::leaky)
                    {
                        underlyingLayers.push_back(std::make_unique<underlying::ActivationLayer<ElementType, underlying::LeakyReLUActivation>>(parameters));
                    }
                    else if (apiLayer.activation == api::ActivationType::relu)
                    {
                        underlyingLayers.push_back(std::make_unique<underlying::ActivationLayer<ElementType, underlying::ReLUActivation>>(parameters));
                    }
                    else
                    {
                        underlyingLayers.push_back(std::make_unique<underlying::ActivationLayer<ElementType, underlying::SigmoidActivation>>(parameters));
                    }
                }
                break;
                case (underlying::LayerType::batchNormalization):
                {
                    auto& apiLayer = LayerAs<api::BatchNormalizationLayer<ElementType>>(layer);
                    auto epsilonSummand = (apiLayer.epsilonSummand == api::EpsilonSummand::variance) ? underlying::EpsilonSummand::Variance : underlying::EpsilonSummand::SqrtVariance;
                    underlyingLayers.push_back(std::make_unique<underlying::BatchNormalizationLayer<ElementType>>(parameters, apiLayer.mean, apiLayer.variance, apiLayer.epsilon, epsilonSummand));
                }
                break;
                case (underlying::LayerType::bias):
                {
                    auto& apiLayer = LayerAs<api::BiasLayer<ElementType>>(layer);
                    underlyingLayers.push_back(std::make_unique<underlying::BiasLayer<ElementType>>(parameters, apiLayer.bias));
                }
                break;
                case (underlying::LayerType::binaryConvolution):
                {
                    auto& apiLayer = LayerAs<api::BinaryConvolutionalLayer<ElementType>>(layer);
                    TensorType weights(apiLayer.weights.rows, apiLayer.weights.columns, apiLayer.weights.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::BinaryConvolutionalLayer<ElementType>>(parameters, apiLayer.convolutionalParameters, weights));
                }
                break;
                case (underlying::LayerType::convolution):
                {
                    auto& apiLayer = LayerAs<api::ConvolutionalLayer<ElementType>>(layer);
                    TensorType weights(apiLayer.weights.rows, apiLayer.weights.columns, apiLayer.weights.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::ConvolutionalLayer<ElementType>>(parameters, apiLayer.convolutionalParameters, weights));
                }
                break;
                case (underlying::LayerType::fullyConnected):
                {
                    auto& apiLayer = LayerAs<api::FullyConnectedLayer<ElementType>>(layer);
                    TensorType weights(apiLayer.weights.rows, apiLayer.weights.columns, apiLayer.weights.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::FullyConnectedLayer<ElementType>>(parameters, weights));
                }
                break;
                case (underlying::LayerType::pooling):
                {
                    auto& apiLayer = LayerAs<api::PoolingLayer<ElementType>>(layer);
                    if (apiLayer.poolingType == api::PoolingType::max)
                    {
                        underlyingLayers.push_back(std::make_unique<underlying::PoolingLayer<ElementType, underlying::MaxPoolingFunction>>(parameters, apiLayer.poolingParameters));
                    }
                    else
                    {
                        underlyingLayers.push_back(std::make_unique<underlying::PoolingLayer<ElementType, underlying::MeanPoolingFunction>>(parameters, apiLayer.poolingParameters));
                    }
                }
                break;
                case (underlying::LayerType::scaling):
                {
                    auto& apiLayer = LayerAs<api::ScalingLayer<ElementType>>(layer);
                    underlyingLayers.push_back(std::make_unique<underlying::ScalingLayer<ElementType>>(parameters, apiLayer.scales));
                }
                break;
                case (underlying::LayerType::softmax):
                {
                    auto& apiLayer = LayerAs<api::SoftmaxLayer<ElementType>>(layer);
                    underlyingLayers.push_back(std::make_unique<underlying::SoftmaxLayer<ElementType>>(parameters));
                }
                break;
                default:
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Encountered unknown layer type in neural network predictor");
                    break;
            }
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Encountered null layer in neural network predictor");
        }
    }

#ifndef SWIG
    template <typename ElementType>
    const ell::predictors::NeuralNetworkPredictor<ElementType>& NeuralNetworkPredictor<ElementType>::GetPredictor() const
    {
        return *_predictor;
    }
#endif
}
}
}
