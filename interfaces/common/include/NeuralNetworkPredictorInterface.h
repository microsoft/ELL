////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorInterface.h (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

// neural network predictor
#include "NeuralNetworkPredictor.h"
#include "Activation.h"

// stl
#include <string>
#include <vector>

#endif

// apis
#include "MathInterface.h"
#include "NeuralLayersInterface.h"

namespace ell
{
namespace api
{
namespace predictors
{
    //
    // API classes for the neural predictor
    //
    template <typename ElementType>
    class NeuralNetworkPredictor
    {
    public:
        using Layer = ell::api::predictors::neural::Layer<ElementType>;
        using LayerShape = ell::api::math::TensorShape;
        using UnderlyingPredictor = ell::predictors::NeuralNetworkPredictor<ElementType>;
        using UnderlyingLayer = typename ell::predictors::neural::Layer<ElementType>;
        using UnderlyingLayers = typename ell::predictors::NeuralNetworkPredictor<ElementType>::Layers;
        using UnderlyingInputParameters = typename ell::predictors::neural::InputLayer<ElementType>::InputParameters;
        using UnderlyingInputLayer = typename ell::predictors::neural::InputLayer<ElementType>;
        using UnderlyingLayerParameters = typename ell::predictors::neural::Layer<ElementType>::LayerParameters;       

        NeuralNetworkPredictor(const std::vector<Layer*>& layers, ElementType scaleFactor=1.0f);
        std::vector<ElementType> Predict(const std::vector<ElementType>& input);
        void RemoveLastLayers(size_t numLayersToRemove);
        LayerShape GetInputShape() const;
        LayerShape GetOutputShape() const;

#ifndef SWIG
        const UnderlyingPredictor& GetPredictor() const;
#endif

private:
    
#ifndef SWIG

        template <typename DerivedLayer>
        static bool LayerIs(Layer* layer);

        template <typename DerivedLayer>
        static auto& LayerAs(Layer* layer);

        static void AddLayer(Layer* layer, const std::unique_ptr<UnderlyingInputLayer>& underlyingInputLayer, UnderlyingLayers& underlyingLayers);

        // Specific layer factory functions
        static std::unique_ptr<UnderlyingLayer> CreateActivationLayer(ell::api::predictors::neural::ActivationLayer<ElementType>& layer, const UnderlyingLayerParameters& parameters);

        static std::unique_ptr<UnderlyingLayer> CreateGRULayer(ell::api::predictors::neural::GRULayer<ElementType>& layer, const UnderlyingLayerParameters& parameters);

        static std::unique_ptr<UnderlyingLayer> CreateLSTMLayer(ell::api::predictors::neural::LSTMLayer<ElementType>& layer, const UnderlyingLayerParameters& parameters);

#endif

        std::shared_ptr<UnderlyingPredictor> _predictor;
    };

#ifndef SWIG
    template<typename ElementType>
    ell::predictors::neural::Activation<ElementType> CreateActivation(ell::api::predictors::neural::ActivationType type);
#endif
}
}
}

#include "../tcc/NeuralNetworkPredictorInterface.tcc"
