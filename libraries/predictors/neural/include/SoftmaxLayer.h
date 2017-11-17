////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftmaxLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> A layer in a neural network that provides a softmax mapping of the input to output such that the output probabilities sum up to 1. </summary>
    template <typename ElementType>
    class SoftmaxLayer : public Layer<ElementType>
    {
    public:
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using Layer<ElementType>::GetOutputMinusPadding;
        using Layer<ElementType>::AssignValues;

        /// <summary> Instantiates an instance of a softmax layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        SoftmaxLayer(const LayerParameters& layerParameters);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        SoftmaxLayer() {}

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::softmax; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("SoftmaxLayer"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    private:
        using Layer<ElementType>::_layerParameters;
        using Layer<ElementType>::_output;
    };

}
}
}

#include "../tcc/SoftmaxLayer.tcc"