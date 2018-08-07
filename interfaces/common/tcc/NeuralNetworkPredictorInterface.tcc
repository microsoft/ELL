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
#include "NeuralLayersInterface.h"

// neural predictor
#include "HardSigmoidActivation.h"
#include "LeakyReLUActivation.h"
#include "MaxPoolingFunction.h"
#include "MeanPoolingFunction.h"
#include "ParametricReLUActivation.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"

// utilities
#include "Exception.h"
#include "Files.h"
#include "JsonArchiver.h"
#include "StringUtil.h"

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

#ifndef SWIG
        
        template<typename ElementType>
        ell::predictors::neural::Activation<ElementType> CreateActivation(neural::ActivationType type)
        {
            std::unique_ptr<ell::predictors::neural::ActivationImpl<ElementType>> activation;
            switch (type)
            {
            case neural::ActivationType::relu:
                activation = std::make_unique<underlying::ReLUActivation<ElementType>>();
                break;
            case neural::ActivationType::leaky:
                activation = std::make_unique<underlying::LeakyReLUActivation<ElementType>>();
                break;
            case neural::ActivationType::hardSigmoid:
                activation = std::make_unique<underlying::HardSigmoidActivation<ElementType>>();
                break;
            case neural::ActivationType::sigmoid:
                activation = std::make_unique<underlying::SigmoidActivation<ElementType>>();
                break;
            case neural::ActivationType::tanh:
                activation = std::make_unique<underlying::TanhActivation<ElementType>>();
                break;
            default:
                throw ell::utilities::InputException(ell::utilities::InputExceptionErrors::invalidArgument, 
                    utilities::FormatString("Encountered unsupported activation type in neural network predictor: %d", static_cast<int>(type)));
            }
            return ell::predictors::neural::Activation<ElementType>(activation);
        }

        //
        // CreateActivationLayer
        //
        template <typename ElementType>
        std::unique_ptr<underlying::Layer<ElementType>> NeuralNetworkPredictor<ElementType>::CreateActivationLayer(neural::ActivationLayer<ElementType>& layer, const UnderlyingLayerParameters& parameters)
        {
            using TensorType = typename underlying::Layer<ElementType>::TensorType;
            using ActivationImplType = ell::predictors::neural::ActivationImpl<ElementType>;

            ell::predictors::neural::Activation<ElementType> activation;
            switch (layer.activation)
            {
            case neural::ActivationType::relu:
            case neural::ActivationType::hardSigmoid:
            case neural::ActivationType::sigmoid:
            case neural::ActivationType::tanh:
                activation = CreateActivation<ElementType>(layer.activation);
                break;
            case neural::ActivationType::leaky:
            {
                ActivationImplType* implementation = nullptr;
                if (NeuralNetworkPredictor<ElementType>::LayerIs<neural::LeakyReLUActivationLayer<ElementType>>(&layer))
                {
                    auto& leakyReluApiLayer = NeuralNetworkPredictor<ElementType>::LayerAs<neural::LeakyReLUActivationLayer<ElementType>>(&layer);
                    implementation = new underlying::LeakyReLUActivation<ElementType>(leakyReluApiLayer._alpha);
                }
                else 
                {
                    implementation = new underlying::LeakyReLUActivation<ElementType>();
                }
                activation = ell::predictors::neural::Activation<ElementType>(implementation);
                break;
            }
            case neural::ActivationType::prelu:
            {
                auto& preluApiLayer = NeuralNetworkPredictor<ElementType>::LayerAs<neural::PReLUActivationLayer<ElementType>>(&layer);
                TensorType alpha(preluApiLayer.alpha.shape.rows, preluApiLayer.alpha.shape.columns, preluApiLayer.alpha.shape.channels, preluApiLayer.alpha.data);
                activation = ell::predictors::neural::Activation<ElementType>(new underlying::ParametricReLUActivation<ElementType>(alpha));
                break;
            }
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                    utilities::FormatString("Encountered unsupported activation type in neural network predictor: %d", static_cast<int>(layer.activation)));
            }

            return std::make_unique<underlying::ActivationLayer<ElementType>>(parameters, activation);
        }

        //
        // CreateGRULayer
        //

        template <typename ElementType>
        std::unique_ptr<underlying::Layer<ElementType>> NeuralNetworkPredictor<ElementType>::CreateGRULayer(neural::GRULayer<ElementType>& layer, const UnderlyingLayerParameters& parameters)
        {
            size_t m = layer.updateWeights.shape.rows;
            size_t n = layer.updateWeights.shape.columns;
            size_t s = layer.updateBias.shape.rows * layer.updateBias.shape.columns * layer.updateBias.shape.channels;
            underlying::GRUParameters<ElementType> gruParameters = { { layer.updateWeights.data.data(), m, n }, { layer.resetWeights.data.data(), m, n }, { layer.hiddenWeights.data.data(), m, n }, { layer.updateBias.data.data(), s }, { layer.resetBias.data.data(), s }, { layer.hiddenBias.data.data(), s } };

            return std::make_unique<underlying::GRULayer<ElementType>>(parameters, gruParameters, CreateActivation<ElementType>(layer.activation), CreateActivation<ElementType>(layer.recurrentActivation));
        }

        //
        // CreateLSTMLayer
        //

        template <typename ElementType>
        std::unique_ptr<underlying::Layer<ElementType>> NeuralNetworkPredictor<ElementType>::CreateLSTMLayer(neural::LSTMLayer<ElementType>& layer, const UnderlyingLayerParameters& parameters)
        {
            size_t m = layer.inputWeights.shape.rows;
            size_t n = layer.inputWeights.shape.columns;
            underlying::LSTMParameters<ElementType> lstmParameters = {
                { layer.inputWeights.data.data(), m, n },
                { layer.forgetMeWeights.data.data(), m, n },
                { layer.candidateWeights.data.data(), m, n },
                { layer.outputWeights.data.data(), m, n },
                { layer.inputBias.data.data(), m },
                { layer.forgetMeBias.data.data(), m },
                { layer.candidateBias.data.data(), m },
                { layer.outputBias.data.data(), m }
            };

            return std::make_unique<underlying::LSTMLayer<ElementType>>(parameters, lstmParameters, 
                CreateActivation<ElementType>(layer.activation), CreateActivation<ElementType>(layer.recurrentActivation));
        }
#endif 

        //
        // API classes for the neural predictor
        //
        template <typename ElementType>
        NeuralNetworkPredictor<ElementType>::NeuralNetworkPredictor(const std::vector<neural::Layer<ElementType>*>& layers, ElementType scaleFactor)
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
        bool NeuralNetworkPredictor<ElementType>::LayerIs(Layer* layer)
        {
            return layer->template Is<DerivedLayer>();
        }

        template <typename ElementType>
        template <typename DerivedLayer>
        auto& NeuralNetworkPredictor<ElementType>::LayerAs(Layer* layer)
        {
            if (layer == nullptr)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Trying to cast null layer");
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
                    auto& apiLayer = LayerAs<neural::ActivationLayer<ElementType>>(layer);
                    underlyingLayers.push_back(CreateActivationLayer(apiLayer, parameters));
                }
                break;
                case (underlying::LayerType::batchNormalization):
                {
                    auto& apiLayer = LayerAs<neural::BatchNormalizationLayer<ElementType>>(layer);
                    auto epsilonSummand = (apiLayer.epsilonSummand == neural::EpsilonSummand::variance) ? underlying::EpsilonSummand::Variance : underlying::EpsilonSummand::SqrtVariance;
                    underlyingLayers.push_back(std::make_unique<underlying::BatchNormalizationLayer<ElementType>>(parameters, apiLayer.mean, apiLayer.variance, apiLayer.epsilon, epsilonSummand));
                }
                break;
                case (underlying::LayerType::bias):
                {
                    auto& apiLayer = LayerAs<neural::BiasLayer<ElementType>>(layer);
                    underlyingLayers.push_back(std::make_unique<underlying::BiasLayer<ElementType>>(parameters, apiLayer.bias));
                }
                break;
                case (underlying::LayerType::binaryConvolution):
                {
                    auto& apiLayer = LayerAs<neural::BinaryConvolutionalLayer<ElementType>>(layer);
                    TensorType weights(apiLayer.weights.shape.rows, apiLayer.weights.shape.columns, apiLayer.weights.shape.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::BinaryConvolutionalLayer<ElementType>>(parameters, apiLayer.convolutionalParameters, weights));
                }
                break;
                case (underlying::LayerType::convolution):
                {
                    auto& apiLayer = LayerAs<neural::ConvolutionalLayer<ElementType>>(layer);
                    TensorType weights(apiLayer.weights.shape.rows, apiLayer.weights.shape.columns, apiLayer.weights.shape.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::ConvolutionalLayer<ElementType>>(parameters, apiLayer.convolutionalParameters, weights));
                }
                break;
                case (underlying::LayerType::fullyConnected):
                {
                    auto& apiLayer = LayerAs<neural::FullyConnectedLayer<ElementType>>(layer);
                    TensorType weights(apiLayer.weights.shape.rows, apiLayer.weights.shape.columns, apiLayer.weights.shape.channels, apiLayer.weights.data);
                    underlyingLayers.push_back(std::make_unique<underlying::FullyConnectedLayer<ElementType>>(parameters, weights));
                }
                break;
                case (underlying::LayerType::lstm):
                {
                    auto& apiLayer = LayerAs<neural::LSTMLayer<ElementType>>(layer);
                    underlyingLayers.push_back(CreateLSTMLayer(apiLayer, parameters));
                }
                break;
                case (underlying::LayerType::gru):
                {
                    auto& apiLayer = LayerAs<neural::GRULayer<ElementType>>(layer);
                    underlyingLayers.push_back(CreateGRULayer(apiLayer, parameters));
                }
                break;
                case (underlying::LayerType::pooling):
                {
                    auto& apiLayer = LayerAs<neural::PoolingLayer<ElementType>>(layer);
                    if (apiLayer.poolingType == neural::PoolingType::max)
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
                    auto& apiLayer = LayerAs<neural::RegionDetectionLayer<ElementType>>(layer);
                    underlyingLayers.push_back(std::make_unique<underlying::RegionDetectionLayer<ElementType>>(parameters, apiLayer.detectionParameters));
                }
                break;
                case (underlying::LayerType::scaling):
                {
                    auto& apiLayer = LayerAs<neural::ScalingLayer<ElementType>>(layer);
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
