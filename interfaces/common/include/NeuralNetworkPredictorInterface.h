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

// stl
#include <string>
#include <vector>

// apis
#include "MathInterface.h"
#include "NeuralLayersInterface.h"
#endif

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
        static auto& LayerAs(Layer* layer);

        static void AddLayer(Layer* layer, const std::unique_ptr<UnderlyingInputLayer>& underlyingInputLayer, UnderlyingLayers& underlyingLayers);

        // Specific layer factory functions
        static std::unique_ptr<UnderlyingLayer> CreateActivationLayer(neural::ActivationLayer<ElementType>& layer, const UnderlyingLayerParameters& parameters);

        template <template <typename> class ActivationFunctionType, template <typename> class RecurrentActivationFunctionType>
        static std::unique_ptr<UnderlyingLayer> CreateGRULayer(neural::GRULayer<ElementType>& layer, const UnderlyingLayerParameters& parameters);
        
        template <template <typename> class ActivationFunctionType>
        static std::unique_ptr<UnderlyingLayer> CreateGRULayer(neural::GRULayer<ElementType>& layer, const UnderlyingLayerParameters& parameters);
    
        static std::unique_ptr<UnderlyingLayer> CreateGRULayer(neural::GRULayer<ElementType>& layer, const UnderlyingLayerParameters& parameters);

#endif

        std::shared_ptr<UnderlyingPredictor> _predictor;
    };
}
}
}

#include "../tcc/NeuralNetworkPredictorInterface.tcc"
