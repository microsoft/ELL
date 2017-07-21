////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputLayer.h (neural)
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
    

    /// <summary> An input layer in a neural network. This is the only layer type that takes input from an external source, and not from the output of another layer.
    /// This must be the first layer in the list of layers that get set on a Neural Predictor.
    /// </summary>
    template <typename ElementType>
    class InputLayer : public Layer<ElementType>
    {
    public:

        using Shape = typename Layer<ElementType>::Shape;
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using VectorType = typename Layer<ElementType>::VectorType;
        using TensorType = typename Layer<ElementType>::TensorType;
        using DataVectorType = typename Layer<ElementType>::DataVectorType;
        using Layer<ElementType>::GetOutputMinusPadding;
        using Layer<ElementType>::NumOutputRowsMinusPadding;
        using Layer<ElementType>::NumOutputColumnsMinusPadding;
        using Layer<ElementType>::NumOutputChannels;
        using Layer<ElementType>::AssignValues;

        /// <summary> Parameters common to all layers. </summary>
        struct InputParameters
        {
            /// <summary> Shape of the input tensor.. </summary>
            Shape inputShape;
            /// <summary> The padding requirements for the input. </summary>
            PaddingParameters inputPaddingParameters;
            /// <summary> The extents of the tensor in canonical row, column, channel order. This size includes padding. </summary>
            Shape outputShape;
            /// <summary> The padding requirements for the output. </summary>
            PaddingParameters outputPaddingParameters;
            /// <summary> The scale factor to apply to each input value. Default is 1 (i.e. no scale). </summary>
            ElementType scale;
        };

        /// <summary> Instantiates an instance of an input layer. </summary>
        ///
        /// <param name="inputParameters"> The parameters for the input layer. </param>
        /// <param name="inputParameters">   </param>
        InputLayer(const InputParameters& inputParameters);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        InputLayer() : _data(0, 0, 0) {}

        /// <summary> Sets the input. </summary>
        ///
        /// <param name="input"> Copies the input vector to the input tensor. </param>
        void SetInput(const DataVectorType& input);

        /// <summary> Gets a writeable reference to the input. </summary>
        ///
        /// <returns> The output tensor. </returns>
        TensorType& GetInput() { return _data; }

        /// <summary> Gets a const reference to the input. </summary>
        ///
        /// <returns> The output tensor. </returns>
        const TensorType& GetInput() const { return _data; }

        /// <summary> Feeds the input forward through the layer. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::input; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("InputLayer"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        using Layer<ElementType>::_layerParameters;
        using Layer<ElementType>::_output;

        VectorType _scale;
        TensorType _data;
    };

}
}
}

#include "../tcc/InputLayer.tcc"

