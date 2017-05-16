////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Layer.h (neural)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "IPredictor.h"

// math
#include "Vector.h"
#include "Tensor.h"
#include "TensorOperations.h"

// data
#include "Dataset.h"

// utilities
#include "IArchivable.h"

// stl
#include <cstddef>
#include <memory>
#include <ostream>

namespace ell
{
namespace predictors
{
namespace neural
{

    /// <summary> Enum that represents the type of neural network layer. </summary>
    enum class LayerType : int
    {
        base,
        activation,
        batchNormalization,
        bias,
        binaryConvolution,
        convolution,
        fullyConnected,
        input,
        pooling,
        scaling,
        softmax,
    };
    static const std::string LayerNames[] = { "Base", "Activation", "BatchNormalization", "Bias", "BinaryConvolution", "Convolution", "FullyConnected", "Input", "Pooling", "Scaling", "Softmax" };

    /// <summary> Enum that represents the type of padding values in a neural network layer. </summary>
    enum class PaddingScheme : int
    {
        zeros,
        minusOnes,
        alternatingZeroAndOnes,
        randomZeroAndOnes,
        min,
        max
    };

    /// <summary> Struct that holds information about the padding, both the scheme to use and the padding size.
    /// The padding size represents padding on either side in the row and column dimensions.
    /// </summary>
    struct PaddingParameters
    {
        PaddingScheme paddingScheme;
        size_t paddingSize;
    };

    // Typical padding parameters
    /// <summary> Padding config alias that represents no padding </summary
    static const PaddingParameters NoPadding{PaddingScheme::zeros, 0};
    /// <summary> Padding config alias that represents a padding of 1 with 0 as the padding value. </summary
    static const PaddingParameters OnePaddingWithZeros{PaddingScheme::zeros, 1};
    /// <summary> Padding config alias that represents a padding of 1 with minimum values as the padding value. </summary
    static const PaddingParameters OnePaddingWithMinimum{PaddingScheme::min, 1};
    /// <summary> Padding config alias that represents a padding of 1 with -1 values as the padding value. </summary
    static const PaddingParameters OnePaddingWithMinusOnes{PaddingScheme::minusOnes, 1};

    /// <summary> Common base class for a layer in a neural network. </summary>
    template <typename ElementType>
    class Layer
    {
    public:
        using Shape = math::Triplet;
        using VectorType = math::ColumnVector<ElementType>;
        using MatrixType = math::RowMatrix<ElementType>;
        using MatrixReferenceType = math::ConstMatrixReference<ElementType, math::MatrixLayout::rowMajor>;
        using TensorType = math::ChannelColumnRowTensor<ElementType>;
        using TensorReferenceType = math::TensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using ConstTensorReferenceType = math::ConstTensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using DataVectorType = data::FloatDataVector;

        /// <summary> Parameters common to all layers, specifying info related to input and output of the layer. </summary>
        struct LayerParameters
        {
            /// <summary> Reference to the input tensor.. </summary>
            ConstTensorReferenceType input;
            /// <summary> The padding requirements for the input. </summary>
            PaddingParameters inputPaddingParameters;
            /// <summary> The extents of the tensor in canonical row, column, channel order. This size includes padding. </summary>
            Shape outputShape;
            /// <summary> The padding requirements for the output. </summary>
            PaddingParameters outputPaddingParameters;
        };

        virtual ~Layer() = default;

        /// <summary> Initializes this class with the required information regarding inputs and outputs. </summary>
        /// <param name="layerParameters"> The parameters for this layer. </param>
        Layer(const LayerParameters& layerParameters);

        /// <summary> Returns a reference to the output tensor. </summary>
        ///
        /// <returns> Reference to the output tensor. </returns>
        ConstTensorReferenceType GetOutput() { return _output; }

        /// <summary> Returns shape of the input tensor. </summary>
        ///
        /// <returns> Shape of the input tensor. </returns>
        virtual Shape GetInputShape() { return _layerParameters.input.GetShape(); }

        /// <summary> Returns shape of the output tensor. </summary>
        ///
        /// <returns> Shape of the output tensor. </returns>
        virtual Shape GetOutputShape() { return _layerParameters.outputShape; };

        /// <summary> Used to get a layer as a specific type. </summary>
        ///
        /// <returns> Reference to the layer as a specific layer type. </returns>
        template <class LayerType>
        LayerType& As() { return *(dynamic_cast<LayerType*>(this)); }

        /// <summary> Computes the output of the layer via a forward feed of the configured input.
        ///           This is a no-op for this layer type. </summary>
        virtual void Compute() {};

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        virtual LayerType GetLayerType() const { return LayerType::base; };

        /// <summary> Prints diagnostic info about the layer to the  output stream. </summary> 
        ///
        /// <param name="os"> The stream that receives the formated output (e.g. std::out) </param>
        /// <param name="maxValuesToPrint"> The maximum number of values from the layer output to include in the info sent to the output stream </param>
        virtual void Print(std::ostream& os, size_t maxValuesToPrint = 100) const;

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver);

    protected:

        /// <summary> Returns a read/write reference to the sub tensor of the output that does not contain padding. </summary>
        ///
        /// <returns> Read/write reference to the output tensor. </returns>
        TensorReferenceType GetOutputMinusPadding();

        /// <summary> Returns number of output rows minus padding. </summary>
        size_t NumOutputRowsMinusPadding() const { return _output.NumRows() - 2 * _layerParameters.outputPaddingParameters.paddingSize; }
        /// <summary> Returns number of output columns minus padding. </summary>
        size_t NumOutputColumnsMinusPadding() const { return _output.NumColumns() - 2 * _layerParameters.outputPaddingParameters.paddingSize; }
        /// <summary> Returns number of output channels. </summary>
        size_t NumOutputChannels() const { return _output.NumChannels(); };

        /// <summary> Sets the initial output values according to the padding scheme. </summary>
        void InitializeOutputValues(TensorType& output, PaddingParameters outputPaddingParameters);

        // Temporary: This method will be removed once the Tensor operations have been modified to to take destination parameters,
        // rather than doing them in place
        void AssignValues(ConstTensorReferenceType& input, TensorReferenceType& output);

        LayerParameters _layerParameters;
        TensorType _output;
    };

}
}
}

#include "../tcc/Layer.tcc"
