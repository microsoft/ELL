////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BatchNormalizationLayer.h (neural)
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
    /// <summary> A layer in a neural network that applies batch normalization to the input. </summary>
    template <typename ElementType>
    class BatchNormalizationLayer : public Layer<ElementType>
    {
    public:
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using VectorType = typename Layer<ElementType>::VectorType;
        using Layer<ElementType>::GetOutputMinusPadding;
        using Layer<ElementType>::NumOutputRowsMinusPadding;
        using Layer<ElementType>::NumOutputColumnsMinusPadding;
        using Layer<ElementType>::NumOutputChannels;
        using Layer<ElementType>::AssignValues;
        
        /// <summary> Instantiates an instance of a batch normalization layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="mean"> The mean values. </param>
        /// <param name="variance"> The variance values. </param>
        BatchNormalizationLayer(const LayerParameters& layerParameters, const VectorType& mean, const VectorType& variance);

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::batchNormalization; }

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

        VectorType _multiplicationValues;
        VectorType _additionValues;
        const ElementType _epsilon = 1.0e-6f; // To ensure non-zero division, this is added to denominator
    };

}
}
}

#include "../tcc/BatchNormalizationLayer.tcc"