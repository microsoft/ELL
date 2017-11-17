////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalingLayer.h (neural)
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
    /// <summary> A layer in a neural network that applies a scale to the input. </summary>
    template <typename ElementType>
    class ScalingLayer : public Layer<ElementType>
    {
    public:
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using VectorType = typename Layer<ElementType>::VectorType;
        using Layer<ElementType>::GetOutputMinusPadding;
        using Layer<ElementType>::AssignValues;

        /// <summary> Instantiates an instance of a scaling layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="bias"> The scaling values to apply to input values. </param>
        ScalingLayer(const LayerParameters& layerParameters, const VectorType& scales);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        ScalingLayer() {}

        /// <summary> Feeds the input forward through the layer. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::scaling; }

        /// <summary> Gets the scaling values. </summary>
        ///
        /// <returns> The scaling values. </returns>
        VectorType GetScale() const { return _scales; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("ScalingLayer"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        using Layer<ElementType>::_layerParameters;
        using Layer<ElementType>::_output;

        VectorType _scales;
    };

}
}
}

#include "../tcc/ScalingLayer.tcc"