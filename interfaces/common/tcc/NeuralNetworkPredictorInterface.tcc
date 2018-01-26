////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorInterface.tcc (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// interfaces
#include "MathInterface.h"
#include "NeuralNetworkPredictorInterface.h"

// neural predictor
#include "HardSigmoidActivation.h"
#include "LeakyReLUActivation.h"
#include "MaxPoolingFunction.h"
#include "MeanPoolingFunction.h"
#include "ParametricReLUActivation.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"

// utilities
#include "Files.h"
#include "JsonArchiver.h"

// stl
#include <ostream>

namespace ell
{
namespace api
{
    namespace predictors
    {
        using LayerShape = ell::api::math::TensorShape;
        namespace underlying = ell::predictors::neural;
        namespace api = ell::api::predictors::neural;

        //
        // CreateActivationLayer
        //
        template <typename ElementType>
        std::unique_ptr<underlying::Layer<ElementType>> NeuralNetworkPredictor<ElementType>::CreateActivationLayer(api::ActivationLayer<ElementType>& layer, const UnderlyingLayerParameters& parameters)
        {
            using TensorType = typename underlying::Layer<ElementType>::TensorType;

            switch (layer.activation)
            {
            case api::ActivationType::relu:
                return std::make_unique<underlying::ActivationLayer<ElementType, underlying::ReLUActivation>>(parameters);
            case api::ActivationType::hardSigmoid:
                return std::make_unique<underlying::ActivationLayer<ElementType, underlying::HardSigmoidActivation>>(parameters);
            case api::ActivationType::leaky:
                return std::make_unique<underlying::ActivationLayer<ElementType, underlying::LeakyReLUActivation>>(parameters);
            case api::ActivationType::sigmoid:
                return std::make_unique<underlying::ActivationLayer<ElementType, underlying::SigmoidActivation>>(parameters);
            case api::ActivationType::tanh:
                return std::make_unique<underlying::ActivationLayer<ElementType, underlying::TanhActivation>>(parameters);
            case api::ActivationType::prelu:
            {
                auto& preluApiLayer = NeuralNetworkPredictor<ElementType>::LayerAs<api::PReLUActivationLayer<ElementType>>(&layer);
                TensorType alpha(preluApiLayer.alpha.shape.rows, preluApiLayer.alpha.shape.columns, preluApiLayer.alpha.shape.channels, preluApiLayer.alpha.data);
                underlying::ParametricReLUActivation<ElementType> prelu(alpha);
                return std::make_unique<underlying::ActivationLayer<ElementType, underlying::ParametricReLUActivation>>(parameters, prelu);
            }
            case api::ActivationType::softmax:
                return std::make_unique<underlying::SoftmaxLayer<ElementType>>(parameters);
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, std::string("Encountered unknown activation type in neural network predictor: ") + std::to_string(static_cast<int>(layer.activation)));
            }
        }

        //
        // CreateGRULayer
        //

        template <typename ElementType>
        template <template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        std::unique_ptr<underlying::Layer<ElementType>> NeuralNetworkPredictor<ElementType>::CreateGRULayer(api::GRULayer<ElementType>& layer, const UnderlyingLayerParameters& parameters)
        {
            size_t m = layer.updateWeights.shape.rows;
            size_t n = layer.updateWeights.shape.columns;
            underlying::GRUParameters<ElementType> gruParameters = { { layer.updateWeights.data.data(), m, n }, { layer.resetWeights.data.data(), m, n }, { layer.hiddenWeights.data.data(), m, n }, { layer.updateBias.data.data(), m }, { layer.resetBias.data.data(), m }, { layer.hiddenBias.data.data(), m } };

            return std::make_unique<underlying::GRULayer<ElementType, ActivationFunctionType, RecurrentActivationFunctionType>>(parameters, gruParameters);
        }

        template <typename ElementType>
        template <template <typename> class ActivationFunctionType>
        std::unique_ptr<underlying::Layer<ElementType>> NeuralNetworkPredictor<ElementType>::CreateGRULayer(api::GRULayer<ElementType>& layer, const UnderlyingLayerParameters& parameters)
        {
            using TensorType = typename underlying::Layer<ElementType>::TensorType;

            switch (layer.recurrentActivation)
            {
            case api::ActivationType::relu:
                return CreateGRULayer<ActivationFunctionType, underlying::ReLUActivation>(layer, parameters);
            case api::ActivationType::leaky:
                return CreateGRULayer<ActivationFunctionType, underlying::LeakyReLUActivation>(layer, parameters);
            case api::ActivationType::sigmoid:
                return CreateGRULayer<ActivationFunctionType, underlying::SigmoidActivation>(layer, parameters);
            case api::ActivationType::hardSigmoid:
                return CreateGRULayer<ActivationFunctionType, underlying::HardSigmoidActivation>(layer, parameters);
            case api::ActivationType::tanh:
                return CreateGRULayer<ActivationFunctionType, underlying::TanhActivation>(layer, parameters);
            case api::ActivationType::prelu:
            {
                auto& preluApiLayer = NeuralNetworkPredictor<ElementType>::LayerAs<api::PReLUActivationLayer<ElementType>>(&layer);
                TensorType alpha(preluApiLayer.alpha.shape.rows, preluApiLayer.alpha.shape.columns, preluApiLayer.alpha.shape.channels, preluApiLayer.alpha.data);
                underlying::ParametricReLUActivation<ElementType> prelu(alpha);
                return std::make_unique<underlying::ActivationLayer<ElementType, underlying::ParametricReLUActivation>>(parameters, prelu);
            }
            case api::ActivationType::softmax:
                return std::make_unique<underlying::SoftmaxLayer<ElementType>>(parameters);
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, std::string("Encountered unknown recurrent activation type in neural network predictor: ") + std::to_string(static_cast<int>(layer.recurrentActivation)));
            }
        }

        template <typename ElementType>
        std::unique_ptr<underlying::Layer<ElementType>> NeuralNetworkPredictor<ElementType>::CreateGRULayer(api::GRULayer<ElementType>& layer, const UnderlyingLayerParameters& parameters)
        {
            using TensorType = typename underlying::Layer<ElementType>::TensorType;

            switch (layer.activation)
            {
            case api::ActivationType::relu:
                return CreateGRULayer<underlying::ReLUActivation>(layer, parameters);
            case api::ActivationType::leaky:
                return CreateGRULayer<underlying::LeakyReLUActivation>(layer, parameters);
            case api::ActivationType::sigmoid:
                return CreateGRULayer<underlying::SigmoidActivation>(layer, parameters);
            case api::ActivationType::hardSigmoid:
                return CreateGRULayer<underlying::HardSigmoidActivation>(layer, parameters);
            case api::ActivationType::tanh:
                return CreateGRULayer<underlying::TanhActivation>(layer, parameters);
            case api::ActivationType::prelu:
            {
                auto& preluApiLayer = NeuralNetworkPredictor<ElementType>::LayerAs<api::PReLUActivationLayer<ElementType>>(&layer);
                TensorType alpha(preluApiLayer.alpha.shape.rows, preluApiLayer.alpha.shape.columns, preluApiLayer.alpha.shape.channels, preluApiLayer.alpha.data);
                underlying::ParametricReLUActivation<ElementType> prelu(alpha);
                return std::make_unique<underlying::ActivationLayer<ElementType, underlying::ParametricReLUActivation>>(parameters, prelu);
            }
            case api::ActivationType::softmax:
                return std::make_unique<underlying::SoftmaxLayer<ElementType>>(parameters);
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, std::string("Encountered unknown activation type in neural network predictor: ") + std::to_string(static_cast<int>(layer.activation)));
            }
        }

        //
        // API classes for the neural predictor
        //
        template <typename ElementType>
        NeuralNetworkPredictor<ElementType>::NeuralNetworkPredictor(const std::vector<ell::api::predictors::neural::Layer<ElementType>*>& layers, ElementType scaleFactor)
        {
            if (layers.size() > 0)
            {
                auto& parameters = layers.front()->parameters;
                // Construct the input layer
                UnderlyingInputParameters inputParameters =
                    {
                        { static_cast<size_t>(parameters.inputShape.rows - (2 * parameters.inputPaddingParameters.paddingSize)), static_cast<size_t>(parameters.inputShape.columns - (2 * parameters.inputPaddingParameters.paddingSize)), static_cast<size_t>(parameters.inputShape.channels) },
                        underlying::NoPadding(),
                        { static_cast<size_t>(parameters.inputShape.rows), static_cast<size_t>(parameters.inputShape.columns), static_cast<size_t>(parameters.inputShape.channels) },
                        parameters.inputPaddingParameters,
                        scaleFactor
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
        std::vector<ElementType> NeuralNetworkPredictor<ElementType>::Predict(const std::vector<ElementType>& input)
        {
            std::vector<ElementType> result;
            // Call the underlying predictor with the specified input.
            // Note that we return a copy and not a reference, since we cannot control what the
            // api caller is going to do with it.
            if (_predictor != nullptr)
            {
                result = _predictor->Predict(input);
            }
            else
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Predictor must be initialized before Predict method is called");
            }
            return result;
        }

        template <typename ElementType>
        void NeuralNetworkPredictor<ElementType>::RemoveLastLayers(size_t numLayersToRemove)
        {
            _predictor->RemoveLastLayers(numLayersToRemove);
        }

        template <typename ElementType>
        LayerShape NeuralNetworkPredictor<ElementType>::GetInputShape() const
        {
            auto shape = _predictor->GetInputShape();
            return ell::api::math::TensorShape::FromMathTensorShape(shape);
        }

        template <typename ElementType>
        LayerShape NeuralNetworkPredictor<ElementType>::GetOutputShape() const
        {
            auto shape = _predictor->GetOutputShape();
            return ell::api::math::TensorShape::FromMathTensorShape(shape);
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
                        { static_cast<size_t>(layer->parameters.outputShape.rows), static_cast<size_t>(layer->parameters.outputShape.columns), static_cast<size_t>(layer->parameters.outputShape.channels) },
                        layer->parameters.outputPaddingParameters,
                    };

                // Instantiate the specific layer type
                underlying::LayerType layerType = layer->GetLayerType();
                switch (layerType)
                {
                case (underlying::LayerType::activation):
                {
                    auto& apiLayer = LayerAs<api::ActivationLayer<ElementType>>(layer);
                    underlyingLayers.push_back(CreateActivationLayer(apiLayer, parameters));
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
                    TensorType weights(apiLayer.weights.shape.rows, apiLayer.weights.shape.columns, apiLayer.weights.shape.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::BinaryConvolutionalLayer<ElementType>>(parameters, apiLayer.convolutionalParameters, weights));
                }
                break;
                case (underlying::LayerType::convolution):
                {
                    auto& apiLayer = LayerAs<api::ConvolutionalLayer<ElementType>>(layer);
                    TensorType weights(apiLayer.weights.shape.rows, apiLayer.weights.shape.columns, apiLayer.weights.shape.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::ConvolutionalLayer<ElementType>>(parameters, apiLayer.convolutionalParameters, weights));
                }
                break;
                case (underlying::LayerType::fullyConnected):
                {
                    auto& apiLayer = LayerAs<api::FullyConnectedLayer<ElementType>>(layer);
                    TensorType weights(apiLayer.weights.shape.rows, apiLayer.weights.shape.columns, apiLayer.weights.shape.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::FullyConnectedLayer<ElementType>>(parameters, weights));
                }
                break;
                case (underlying::LayerType::gru):
                {
                    auto& apiLayer = LayerAs<api::GRULayer<ElementType>>(layer);
                    underlyingLayers.push_back(CreateGRULayer(apiLayer, parameters));
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
                case (underlying::LayerType::region):
                {
                    auto& apiLayer = LayerAs<api::RegionDetectionLayer<ElementType>>(layer);
                    underlyingLayers.push_back(std::make_unique<underlying::RegionDetectionLayer<ElementType>>(parameters, apiLayer.detectionParameters));
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
