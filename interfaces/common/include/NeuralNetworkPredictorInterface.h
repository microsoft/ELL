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

    //////////////////////////////////////////////////////////////////////////
    // Api classes for the neural predictor
    //////////////////////////////////////////////////////////////////////////
    template <typename ElementType>
    class NeuralNetworkPredictor
    {
    public:
        using Layer = ell::api::predictors::neural::Layer<ElementType>;
        using UnderlyingPredictor = ell::predictors::NeuralNetworkPredictor<ElementType>;
        using UnderlyingLayers = typename ell::predictors::NeuralNetworkPredictor<ElementType>::Layers;
        using UnderlyingInputParameters = typename ell::predictors::neural::InputLayer<ElementType>::InputParameters;
        using UnderlyingInputLayer = typename ell::predictors::neural::InputLayer<ElementType>;

        NeuralNetworkPredictor(const std::vector<Layer*>& layers);
        std::vector<ElementType> Predict(const std::vector<double>& input);
        neural::LayerShape GetInputShape() const;
        neural::LayerShape GetOutputShape() const;

#ifndef SWIG
        const UnderlyingPredictor& GetPredictor() const;
#endif

    private:
#ifndef SWIG
        template <typename DerivedLayer>
        static auto& LayerAs(Layer* layer);
#endif

        static void AddLayer(Layer* layer, const std::unique_ptr<UnderlyingInputLayer>& underlyingInputLayer, UnderlyingLayers& underlyingLayers);

        std::shared_ptr<UnderlyingPredictor> _predictor;
    };
}
}
}

#include "../tcc/NeuralNetworkPredictorInterface.tcc"
