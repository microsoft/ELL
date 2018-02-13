////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayer.h (neural)
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
    /// <summary> The method for performing convolutions. </summary>
    enum class ConvolutionMethod : int
    {
        /// <summary> Normal method of doing convolution via reshaping input into columns and performing a gemm operation. </summary>
        unrolled = 0,
        /// <summary> A different method of doing convolution which avoids reshaping the input, and uses gemm on smaller matrices with diagonal sums to create output. </summary>
        diagonal = 1,
        /// <summary> A simple, straightforward nested-loop implementation. </summary>
        simple,
    };

    /// <summary> Specifies the hyper parameters of the convolutional layer. </summary>
    struct ConvolutionalParameters
    {
        /// <summary> Width and height of the receptive field that is slid over the input. </summary>
        size_t receptiveField;

        /// <summary> Number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </summary>
        size_t stride;

        /// <summary> Method for doing convolution. </summary>
        ConvolutionMethod method;

        /// <summary> Number of filters to batch at a time when using the Diagonal method. </summary>
        size_t numFiltersAtATime;
    };

    /// <summary> A layer in a neural network that implements a fully connected layer, meaning all nodes in this layer are connected to all
    /// outputs of the previous layer (which are the inputs of this layer). </summary>
    template <typename ElementType>
    class ConvolutionalLayer : public Layer<ElementType>
    {
    public:
        using LayerParameters = typename Layer<ElementType>::LayerParameters;
        using MatrixType = typename Layer<ElementType>::MatrixType;
        using TensorType = typename Layer<ElementType>::TensorType;
        using ConstTensorReferenceType = typename Layer<ElementType>::ConstTensorReferenceType;
        using Layer<ElementType>::GetOutputMinusPadding;
        using Layer<ElementType>::NumOutputRowsMinusPadding;
        using Layer<ElementType>::NumOutputColumnsMinusPadding;
        using Layer<ElementType>::NumOutputChannels;

        /// <summary> Instantiates an instance of a convolutional layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="convolutionalParameters"> The hyperparameters for this convolutional layer. </param>
        /// <param name="weights"> The set of weights to apply. </param>
        ConvolutionalLayer(const LayerParameters& layerParameters, const ConvolutionalParameters& convolutionalParameters, TensorType weights);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        ConvolutionalLayer() : _weights(math::IntegerTriplet{0, 0, 0}), _shapedInput(0, 0), _weightsMatrix(0, 0), _outputMatrix(0 ,0){}

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::convolution; }

        /// <summary> Get the parameters used to control convolution. </summary>
        ///
        /// <returns> A ConvolutionalParameters struct. </returns>
        const ConvolutionalParameters& GetConvolutionalParameters() const { return _convolutionalParameters; }

        /// <summary> Get the weights for the convolution filters. </summary>
        ///
        /// <returns> The weights, packed into a Tensor. </returns>
        const TensorType& GetWeights() const { return _weights; }

        /// <summary> Get the weights for the convolution filters. </summary>
        ///
        /// <returns> The weights, packed into a Matrix. </returns>
        const MatrixType& GetWeightsMatrix() const { return _weightsMatrix; } // Doesn't work

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("ConvolutionalLayer"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // Fills a matrix (backed by the array outputMatrix) where the columns the set of input values corresponding to a filter, stretched into a vector.
        // The number of columns is equal to the number of locations that a filter is slide over the input tensor.
        void ReceptiveFieldToColumns(ConstTensorReferenceType input, MatrixType& shapedInput);
        void ComputeWeightsMatrix();
        void InitializeIOMatrices();

        using Layer<ElementType>::_layerParameters;
        using Layer<ElementType>::_output;

        ConvolutionalParameters _convolutionalParameters;
        TensorType _weights;

        MatrixType _shapedInput;
        MatrixType _weightsMatrix;
        MatrixType _outputMatrix;
    };

}
}
}

#include "../tcc/ConvolutionalLayer.tcc"
