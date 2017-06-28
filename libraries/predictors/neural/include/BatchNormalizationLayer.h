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

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        BatchNormalizationLayer() {}

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::batchNormalization; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("BatchNormalizationLayer"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        /// <summary> Returns the value to scale the output by. </summary>
        ///
        /// <returns> The value to scale the output by. </returns>
        const VectorType& GetScale() const { return _multiplicationValues; }

        /// <summary> Returns the value to offset the output by. </summary>
        ///
        /// <returns> The value to offset the output by. </returns>
        const VectorType& GetBias() const { return _additionValues; }

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
