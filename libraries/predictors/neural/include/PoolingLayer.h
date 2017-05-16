////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PoolingLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Layer.h"

// math
#include "Matrix.h"

// stl
#include <vector>

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> Specifies the hyper parameters of the convolutional layer. </summary>
    struct PoolingParameters
    {
        /// <summary> Width and height of the pooling field that is slid over the input. </summary>
        size_t poolingSize;

        /// <summary> Number of elements to move/jump when sliding over the input. Often this is the same as poolingSize. </summary>
        size_t stride;
    };

    /// <summary> A layer in a neural network that implements pooling. </summary>
    template <typename ElementType, template <typename> class PoolingFunctionType>
    class PoolingLayer : public Layer<ElementType>
    {
    public:

        /// <summary> Instantiates an instance of a pooling layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="poolingParameters"> Specifies the pooling characteristics of the layer. </param>
        PoolingLayer(const LayerParameters& layerParameters, PoolingParameters poolingParameters);

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::pooling; }

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

        PoolingParameters _poolingParameters;
    };

}
}
}

#include "../tcc/PoolingLayer.tcc"