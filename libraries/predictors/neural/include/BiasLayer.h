////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BiasLayer.h (neural)
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
    /// <summary> A layer in a neural network that applies a bias to the input. </summary>
    template <typename ElementType>
    class BiasLayer : public Layer<ElementType>
    {
    public:
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using VectorType = typename Layer<ElementType>::VectorType;
        using Layer<ElementType>::GetOutputMinusPadding;
        using Layer<ElementType>::NumOutputChannels;
        using Layer<ElementType>::AssignValues;

        /// <summary> Instantiates an instance of a bias layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="bias"> The bias values to apply to input values. </param>
        BiasLayer(const LayerParameters& layerParameters, const VectorType& bias);

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::bias; }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        using Layer<ElementType>::_layerParameters;
        using Layer<ElementType>::_output;

        VectorType _bias;
    };

}
}
}

#include "../tcc/BiasLayer.tcc"