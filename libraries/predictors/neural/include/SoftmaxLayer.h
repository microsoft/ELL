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

        /// <summary> Instantiates an instance of a softmax layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        SoftmaxLayer(const LayerParameters& layerParameters);

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::softmax; }

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
    };

}
}
}

#include "../tcc/SoftmaxLayer.tcc"