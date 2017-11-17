////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationLayer.h (neural)
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
    /// <summary> A layer in a neural network that applies an activation function to the input. </summary>
    template <typename ElementType, template <typename> class ActivationFunctionType>
    class ActivationLayer : public Layer<ElementType>
    {
    public:
        using ActivationFunction = ActivationFunctionType<ElementType>;
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using Layer<ElementType>::GetOutputMinusPadding;

        /// <summary> Instantiates an instance of an activation layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        ActivationLayer(const LayerParameters& layerParameters);

        /// <summary> Instantiates an instance of an activation layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="activationFunction"> The activation function. </param>
        ActivationLayer(const LayerParameters& layerParameters, ActivationFunctionType<ElementType> activation);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        ActivationLayer() {}

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::activation; }

        /// <summary> Gets the activation function. </summary>
        ///
        /// <returns> A const reference to the activation function. </returns>
        const ActivationFunctionType<ElementType>& GetActivationFunction() const { return _activation; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType, ActivationFunctionType<ElementType>>("ActivationLayer"); }

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

        ActivationFunctionType<ElementType> _activation;
    };

}
}
}

#include "../tcc/ActivationLayer.tcc"

