////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryConvolutionalLayer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Layer.h"
#include "ConvolutionalLayer.h"

// math
#include "Matrix.h"

namespace ell
{
namespace predictors
{
namespace neural
{

    /// <summary> The method for performing binary convolutions. </summary>
    enum class BinaryConvolutionMethod : int
    {
        /// <summary> Perform the binary convolution as a real-valued GEMM operation (e.g. values are -1.0 and 1.0). </summary>
        gemm = 0,
        /// <summary> Perform binary convolution as bitwise operations. </summary>
        bitwise = 1
    };

    /// <summary> Specifies the hyper parameters of the convolutional layer. </summary>
    struct BinaryConvolutionalParameters
    {
        /// <summary> Width and height of the receptive field that is slid over the input. </summary>
        size_t receptiveField;

        /// <summary> Number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </summary>
        size_t stride;

        /// <summary> Method for doing convolution. </summary>
        BinaryConvolutionMethod method;
    };

    /// <summary> A layer in a neural network that implements a binarized convolutional layer, where operations will occur
    /// on binarized input with binarized weights. </summary>
    template <typename ElementType>
    class BinaryConvolutionalLayer : public Layer<ElementType>
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
        
        /// <summary> Instantiates an instance of a binarized convolutional layer. </summary>
        ///
        /// <param name="layerParameters"> The parameters common to every layer. </param>
        /// <param name="convolutionalParameters"> The hyperparameters for this convolutional layer. </param>
        /// <param name="weights"> The set of weights to apply. </param>
        BinaryConvolutionalLayer(const LayerParameters& layerParameters, const BinaryConvolutionalParameters& convolutionalParameters, const ConstTensorReferenceType& weights);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        BinaryConvolutionalLayer() : _realValuedShapedInput(0, 0), _realValuedWeightsMatrix(0, 0), _realValuedOutputMatrix(0, 0) {}

        /// <summary> Feeds the input forward through the layer and returns a reference to the output. </summary>
        void Compute() override;

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        LayerType GetLayerType() const override { return LayerType::binaryConvolution; }

        /// <summary> Get the parameters used to control convolution. </summary>
        ///
        /// <returns> A ConvolutionalParameters struct. </returns>
        const BinaryConvolutionalParameters& GetConvolutionalParameters() const { return _convolutionalParameters; }

        /// <summary> Get the weights for the convolution filters. </summary>
        ///
        /// <returns> The weights, packed into a Tensor. </returns>
        const MatrixType& GetRealFilterWeights() const { return _realValuedWeightsMatrix; }

        /// <summary> Get the weights for the convolution filters, packed as bits. </summary>
        ///
        /// <returns> The weights, packed as bits. </returns>
        const std::vector<std::vector<uint64_t>> GetCompressedFilterWeights() const { return _binarizedWeights; }

        /// <summary> Get the means for the convolution filters. </summary>
        ///
        /// <returns> The means of the convolution filters. </returns>
        const std::vector<ElementType>& GetFilterMeans() const { return _filterMeans; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("BinaryConvolutionalLayer"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // Fills a vector of vectors where each row is the set of input values corresponding to a filter, stretched into a vector.
        // The number of vectors is equal to the number of locations that the filter is slid over the input tensor.
        void ReceptiveFieldToBinaryRows(ConstTensorReferenceType input, std::vector<std::vector<uint64_t>>& shapedInput);

        // Fills a matrix (backed by the array outputMatrix) where the columns the set of input values corresponding to a filter, stretched into a vector.
        // The number of columns is equal to the number of locations that a filter is slide over the input tensor.
        void ReceptiveFieldToColumns(ConstTensorReferenceType input, MatrixType& shapedInput);

        void ComputeWeightsMatrices(const ConstTensorReferenceType& weights);
        void ComputeRealValuedWeightsMatrix();
        void InitializeIOMatrices();

        using Layer<ElementType>::_layerParameters;
        using Layer<ElementType>::_output;

        constexpr static size_t _binaryElementSize = 64;
        BinaryConvolutionalParameters _convolutionalParameters;
        std::vector<std::vector<uint64_t>> _binarizedShapedInput;
        std::vector<std::vector<uint64_t>> _binarizedWeights;
        std::vector<ElementType> _filterMeans;

        MatrixType _realValuedShapedInput;
        MatrixType _realValuedWeightsMatrix;
        MatrixType _realValuedOutputMatrix;
    };
}
}
}

#include "../tcc/BinaryConvolutionalLayer.tcc"