////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Activation.h"
#include "Layer.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> A layer in a neural network that applies an activation function to the input. </summary>
    template <typename ElementType>
    class ActivationLayer : public Layer<ElementType>
    {
    public:
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using Layer<ElementType>::GetOutputMinusPadding;
        using ActivationType = Activation<ElementType>;

        /// <summary> Instantiates an instance of an activation layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="activation"> The activation function. </param>
        ActivationLayer(const LayerParameters& layerParameters, const ActivationType& activation);

        /// <summary> Make a copy of this layer. </summary>
        ActivationLayer(const ActivationLayer& other);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        ActivationLayer() = default;

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::activation; }

        /// <summary> Gets the activation function. </summary>
        ///
        /// <returns> A const reference to the activation function. </returns>
        const ActivationType& GetActivationFunction() const { return _activation; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("ActivationLayer"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void ValidateDimensions();

        using Layer<ElementType>::_layerParameters;
        using Layer<ElementType>::_output;

        ActivationType _activation;
    };

}
}
}

#include "../tcc/ActivationLayer.tcc"

