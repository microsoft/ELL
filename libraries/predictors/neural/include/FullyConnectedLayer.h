////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FullyConnectedLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Layer.h"

// math
#include "Matrix.h"

namespace ell
{
namespace predictors
{
namespace neural
{
    /// <summary> A layer in a neural network that implements a fully connected layer, meaning all neurons in this layer are connected to all
    /// outputs of the previous layer (which are the inputs of this layer). </summary>
    template <typename ElementType>
    class FullyConnectedLayer : public Layer<ElementType>
    {
    public:
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using VectorType = typename Layer<ElementType>::VectorType;
        using MatrixType = typename Layer<ElementType>::MatrixType;
        using ConstMatrixReferenceType = typename Layer<ElementType>::ConstMatrixReferenceType;
        using ConstTensorReferenceType = typename Layer<ElementType>::ConstTensorReferenceType;
        using Layer<ElementType>::GetOutputMinusPadding;
        using Layer<ElementType>::NumOutputRowsMinusPadding;
        using Layer<ElementType>::NumOutputColumnsMinusPadding;
        using Layer<ElementType>::NumOutputChannels;

        /// <summary> Instantiates an instance of a fully connected layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="weights"> The weights to apply as a matrix in rowMajor order, where number of rows equals output neurons
        /// and columns represent input (in logical Tensor order: row, column, channel).  </param>
        FullyConnectedLayer(const LayerParameters& layerParameters, ConstMatrixReferenceType& weights);

        /// <summary> Instantiates an instance of a fully connected layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="weights"> The weights to apply as stacked Tensors. Each sub-tensor Tensor is the same size as the input,
        /// and the number of tensors stacked (in row dimension) equals the number of outputs in logical Tensor order (row, column, channel).  </param>
        FullyConnectedLayer(const LayerParameters& layerParameters, ConstTensorReferenceType& weights);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        FullyConnectedLayer() : _weights(0,0) {}

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::fullyConnected; }

        /// <summary> Gets the weights </summary>
        ///
        /// <returns> A matrix with the weights for this layer </returns>
        const MatrixType& GetWeights() const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("FullyConnectedLayer"); }

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

        MatrixType _weights;
        VectorType _shapedInput;
        VectorType _outputVector;
    };

}
}
}

#include "../tcc/FullyConnectedLayer.tcc"