////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralLayersInterface.h (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

// math
#include "Tensor.h"

// neural network
#include "Layer.h"
#include "MathInterface.h"
#include "ActivationLayer.h"
#include "BatchNormalizationLayer.h"
#include "BiasLayer.h"
#include "BinaryConvolutionalLayer.h"
#include "ConvolutionalLayer.h"
#include "FullyConnectedLayer.h"
#include "GRULayer.h"
#include "InputLayer.h"
#include "LSTMLayer.h"
#include "PoolingLayer.h"
#include "RegionDetectionLayer.h"
#include "ScalingLayer.h"

// stl
#include <array>
#include <string>
#include <vector>

#endif

// Helper to define a value that is exposed as read-only in the API, but is not
// const from the underlying code side.
#ifdef SWIG
#define API_READONLY(statement) \
    %immutable;                 \
    statement;                  \
    %mutable
#else
#define API_READONLY(statement) \
    statement;
#endif

namespace ell
{
namespace api
{
namespace predictors
{
namespace neural
{

    // Using clauses. These classes will be wrapped separately.
    using LayerType = ell::predictors::neural::LayerType;
    using PaddingScheme = ell::predictors::neural::PaddingScheme;
    using PaddingParameters = ell::predictors::neural::PaddingParameters;
    using LayerShape = ell::api::math::TensorShape;

    //////////////////////////////////////////////////////////////////////////
    // Common types used by the neural layers
    //////////////////////////////////////////////////////////////////////////
    struct LayerParameters
    {
        LayerShape inputShape;
        PaddingParameters inputPaddingParameters;
        LayerShape outputShape;
        PaddingParameters outputPaddingParameters;
    };

    //////////////////////////////////////////////////////////////////////////
    // Api classes for the neural layers
    //////////////////////////////////////////////////////////////////////////

    // Api projection for the layers base class
    template <typename ElementType>
    class Layer
    {
    public:
        Layer(const LayerParameters& layerParameters)
            : parameters(layerParameters) {}
        virtual ~Layer() {}

        virtual LayerType GetLayerType() const = 0;

        template <class LayerType>
        LayerType& As()
        {
            return *(dynamic_cast<LayerType*>(this));
        }

        template <class LayerType>
        bool Is()
        {
            return dynamic_cast<LayerType*>(this) != nullptr;
        }

        const LayerParameters parameters;
    };

    // Api projections for ActivationLayer
    enum class ActivationType : int
    {
        relu,
        leaky,
        sigmoid,
        tanh,
        softmax,
        prelu,
        hardSigmoid
    };

    template <typename ElementType>
    class ActivationLayer : public Layer<ElementType>
    {
    public:
        ActivationLayer(const LayerParameters& layerParameters, ActivationType activation)
            : Layer<ElementType>(layerParameters), activation(activation)
        {
        }

        LayerType GetLayerType() const override { return LayerType::activation; }

        const ActivationType activation;
    };

    // Api projection for PReLUActivationLayer
    template <typename ElementType>
    class PReLUActivationLayer : public ActivationLayer<ElementType>
    {
    public:
        PReLUActivationLayer(const LayerParameters& layerParameters, const ell::api::math::Tensor<ElementType>& alphaTensor)
            : ActivationLayer<ElementType>(layerParameters, ActivationType::prelu), alpha(alphaTensor)
        {
        }

        API_READONLY(ell::api::math::Tensor<ElementType> alpha);
    };

    // Api projection for BatchNormalizationLayer
    enum class EpsilonSummand : int
    {
        variance,
        sqrtVariance
    };

    template <typename ElementType>
    class BatchNormalizationLayer : public Layer<ElementType>
    {
    public:
        BatchNormalizationLayer(const LayerParameters& layerParameters, const std::vector<ElementType>& mean, const std::vector<ElementType>& variance, ElementType epsilon, EpsilonSummand epsilonSummand)
            : Layer<ElementType>(layerParameters), mean(mean), variance(variance), epsilon(epsilon), epsilonSummand(epsilonSummand)
        {
        }

        LayerType GetLayerType() const override { return LayerType::batchNormalization; }

        const std::vector<ElementType> mean;
        const std::vector<ElementType> variance;
        const ElementType epsilon;
        const EpsilonSummand epsilonSummand;
    };

    // Api projection for BiasLayer
    template <typename ElementType>
    class BiasLayer : public Layer<ElementType>
    {
    public:
        BiasLayer(const LayerParameters& layerParameters, const std::vector<ElementType>& bias)
            : Layer<ElementType>(layerParameters), bias(bias)
        {
        }

        LayerType GetLayerType() const override { return LayerType::bias; }

        const std::vector<ElementType> bias;
    };

    // Api projections for BinaryConvolutionalLayer
    using BinaryConvolutionMethod = ell::predictors::neural::BinaryConvolutionMethod;
    using BinaryConvolutionalParameters = ell::predictors::neural::BinaryConvolutionalParameters;

    template <typename ElementType>
    class BinaryConvolutionalLayer : public Layer<ElementType>
    {
    public:
        BinaryConvolutionalLayer(const LayerParameters& layerParameters, const BinaryConvolutionalParameters& convolutionalParameters, const ell::api::math::Tensor<ElementType>& weightsTensor)
            : Layer<ElementType>(layerParameters), weights(weightsTensor.data, weightsTensor.shape.rows, weightsTensor.shape.columns, weightsTensor.shape.channels), convolutionalParameters(convolutionalParameters)
        {
        }

        LayerType GetLayerType() const override { return LayerType::binaryConvolution; }

        API_READONLY(ell::api::math::Tensor<ElementType> weights);
        const BinaryConvolutionalParameters convolutionalParameters;
    };

    // Api projections for ConvolutionalLayer
    using ConvolutionMethod = ell::predictors::neural::ConvolutionMethod;
    using ConvolutionalParameters = ell::predictors::neural::ConvolutionalParameters;

    template <typename ElementType>
    class ConvolutionalLayer : public Layer<ElementType>
    {
    public:
        ConvolutionalLayer(const LayerParameters& layerParameters, const ConvolutionalParameters& convolutionalParameters, const ell::api::math::Tensor<ElementType>& weightsTensor)
            : Layer<ElementType>(layerParameters), weights(weightsTensor.data, weightsTensor.shape.rows, weightsTensor.shape.columns, weightsTensor.shape.channels), convolutionalParameters(convolutionalParameters)
        {
        }

        LayerType GetLayerType() const override { return LayerType::convolution; }

        API_READONLY(ell::api::math::Tensor<ElementType> weights);
        const ConvolutionalParameters convolutionalParameters;
    };

    // Api projections for FullyConnectedLayer
    template <typename ElementType>
    class FullyConnectedLayer : public Layer<ElementType>
    {
    public:
        FullyConnectedLayer(const LayerParameters& layerParameters, const ell::api::math::Tensor<ElementType>& weightsTensor)
            : Layer<ElementType>(layerParameters), weights(weightsTensor.data, weightsTensor.shape.rows, weightsTensor.shape.columns, weightsTensor.shape.channels)
        {
        }

        LayerType GetLayerType() const override { return LayerType::fullyConnected; }

        API_READONLY(ell::api::math::Tensor<ElementType> weights);
    };

    // Api projections for GRULayer
    template <typename ElementType>
    class GRULayer : public Layer<ElementType>
    {
    public:
        GRULayer(const LayerParameters& layerParameters,
                 const ell::api::math::Tensor<ElementType>& updateWeightsTensor,
                 const ell::api::math::Tensor<ElementType>& resetWeightsTensor,
                 const ell::api::math::Tensor<ElementType>& hiddenWeightsTensor,
                 const ell::api::math::Tensor<ElementType>& updateBiasTensor,
                 const ell::api::math::Tensor<ElementType>& resetBiasTensor,
                 const ell::api::math::Tensor<ElementType>& hiddenBiasTensor,
                 ActivationType activation,
                 ActivationType recurrentActivation)
            : Layer<ElementType>(layerParameters),
            updateWeights(updateWeightsTensor),
            resetWeights(resetWeightsTensor),
            hiddenWeights(hiddenWeightsTensor),
            updateBias(updateBiasTensor),
            resetBias(resetBiasTensor),
            hiddenBias(hiddenBiasTensor),
            activation(activation), recurrentActivation(recurrentActivation)
        {
        }

        LayerType GetLayerType() const override { return LayerType::gru; }

        API_READONLY(ell::api::math::Tensor<ElementType> updateWeights);
        API_READONLY(ell::api::math::Tensor<ElementType> resetWeights);
        API_READONLY(ell::api::math::Tensor<ElementType> hiddenWeights);
        API_READONLY(ell::api::math::Tensor<ElementType> updateBias);
        API_READONLY(ell::api::math::Tensor<ElementType> resetBias);
        API_READONLY(ell::api::math::Tensor<ElementType> hiddenBias);
        ActivationType activation;
        ActivationType recurrentActivation;
    };

    // Api projections for LSTMLayer
    template <typename ElementType>
    class LSTMLayer : public Layer<ElementType>
    {
    public:
        LSTMLayer(const LayerParameters& layerParameters, const ell::api::math::Tensor<ElementType>& weightsTensor)
            : Layer<ElementType>(layerParameters), weights(weightsTensor.data, weightsTensor.shape.rows, weightsTensor.shape.columns, weightsTensor.shape.channels)
        {
        }

        LayerType GetLayerType() const override { return LayerType::lstm; }

        API_READONLY(ell::api::math::Tensor<ElementType> weights);
    };

    // Api projections for PoolingLayer
    using PoolingParameters = ell::predictors::neural::PoolingParameters;

    enum class PoolingType : int
    {
        max,
        mean,
    };

    template <typename ElementType>
    class PoolingLayer : public Layer<ElementType>
    {
    public:
        PoolingLayer(const LayerParameters& layerParameters, const PoolingParameters& poolingParameters, PoolingType poolingType)
            : Layer<ElementType>(layerParameters), poolingType(poolingType), poolingParameters(poolingParameters)
        {
        }

        LayerType GetLayerType() const override { return LayerType::pooling; }

        const PoolingType poolingType;
        const PoolingParameters poolingParameters;
    };

    // Api projection for RegionDetectionLayer
    using RegionDetectionParameters = ell::predictors::neural::RegionDetectionParameters;

    template <typename ElementType>
    class RegionDetectionLayer : public Layer<ElementType>
    {
    public:
        RegionDetectionLayer(const LayerParameters& layerParameters, const RegionDetectionParameters& detectionParameters)
            : Layer<ElementType>(layerParameters), detectionParameters(detectionParameters)
        {
        }

        LayerType GetLayerType() const override { return LayerType::region; }

        const RegionDetectionParameters detectionParameters;
    };

    // Api projection for SoftmaxLayer
    template <typename ElementType>
    class SoftmaxLayer : public Layer<ElementType>
    {
    public:
        SoftmaxLayer(const LayerParameters& layerParameters)
            : Layer<ElementType>(layerParameters)
        {
        }

        LayerType GetLayerType() const override { return LayerType::softmax; }
    };

    // Api projection for ScalingLayer
    template <typename ElementType>
    class ScalingLayer : public Layer<ElementType>
    {
    public:
        ScalingLayer(const LayerParameters& layerParameters, const std::vector<ElementType>& scales)
            : Layer<ElementType>(layerParameters), scales(scales)
        {
        }

        LayerType GetLayerType() const override { return LayerType::scaling; }

        const std::vector<ElementType> scales;
    };
}
}
}
}
