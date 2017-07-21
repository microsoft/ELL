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
    /// <summary> Function to return parameters representing no padding </summary
    static PaddingParameters NoPadding() { return { PaddingScheme::zeros, 0 }; }

    /// <summary> Function to return parameters that represent padding the specified pixel width with zeros. </summary
    static PaddingParameters ZeroPadding(size_t width) { return { PaddingScheme::zeros, width }; }

    /// <summary> Function to return parameters that represent padding the specified pixel width with the minimum value. </summary
    static const PaddingParameters MinPadding(size_t width) { return { PaddingScheme::min, width }; }

    /// <summary> Function to return parameters that represent padding the specified pixel width with -1. </summary
    static const PaddingParameters MinusOnePadding(size_t width) { return { PaddingScheme::minusOnes, width }; }

    /// <summary> Helper function to determine if a PaddingParameters struct represents no padding </summary>
    static bool HasPadding(const PaddingParameters& padding) { return padding.paddingSize != 0; }

    /// <summary> Get the padding value to fill with </summary>
    template <typename ValueType>
    ValueType GetPaddingValue(PaddingScheme paddingScheme);

    /// <summary> Common base class for a layer in a neural network. </summary>
    template <typename ElementType>
    class Layer : public utilities::IArchivable
    {
    public:
        using Shape = math::Triplet;
        using VectorType = math::ColumnVector<ElementType>;
        using MatrixType = math::RowMatrix<ElementType>;
        using MatrixReferenceType = math::ConstMatrixReference<ElementType, math::MatrixLayout::rowMajor>;
        using TensorType = math::Tensor<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using TensorReferenceType = math::TensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using ConstTensorReferenceType = math::ConstTensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using DataVectorType = data::FloatDataVector;

        /// <summary> Parameters common to all layers, specifying info related to input and output of the layer. </summary>
        struct LayerParameters
        {
            /// <summary> Reference to the input tensor. Its size does not include the padding.</summary>
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
        ///
        /// <param name="layerParameters"> The parameters for this layer. </param>
        Layer(const LayerParameters& layerParameters);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        Layer()
            : _layerParameters{ math::Triplet{ 0, 0, 0 }, NoPadding(), { 0, 0, 0 }, NoPadding() }, _output(math::Triplet{ 0, 0, 0 }) {}

        /// <summary> Returns a reference to the output tensor. </summary>
        ///
        /// <returns> Reference to the output tensor. </returns>
        ConstTensorReferenceType GetOutput() const { return _output; }

        /// <summary> Returns shape of the input tensor, with padding added. </summary>
        ///
        /// <returns> Shape of the input tensor. </returns>
        virtual Shape GetInputShape() const { return _layerParameters.input.GetShape(); }

        /// <summary> Returns shape of the active area of the input tensor. </summary>
        ///
        /// <returns> Shape of the input tensor. </returns>
        virtual Shape GetInputShapeMinusPadding() const;

        /// <summary> Returns shape of the output tensor, with padding added. </summary>
        ///
        /// <returns> Shape of the output tensor. </returns>
        virtual Shape GetOutputShape() const { return _layerParameters.outputShape; };

        /// <summary> Returns shape of the active area of the output tensor. </summary>
        ///
        /// <returns> Shape of the output tensor. </returns>
        virtual Shape GetOutputShapeMinusPadding() const;

        /// <summary> Indicates if a layer is a specific type. </summary>
        ///
        /// <returns> `true` if the layer is of the queried layer type. </returns>
        template <class LayerType>
        bool IsA() const { return dynamic_cast<LayerType*>(this) != nullptr; }

        /// <summary> Used to get a layer as a specific type. </summary>
        ///
        /// <returns> Reference to the layer as a specific layer type. </returns>
        template <class LayerType>
        LayerType& As() { return *(dynamic_cast<LayerType*>(this)); }

        /// <summary> Computes the output of the layer via a forward feed of the configured input.
        ///           This is a no-op for this layer type. </summary>
        virtual void Compute(){};

        /// <summary> Indicates the kind of layer. </summary>
        ///
        /// <returns> An enum indicating the layer type. </returns>
        virtual LayerType GetLayerType() const { return LayerType::base; };

        /// <summary> Returns the layer parameters. </summary>
        ///
        /// <returns> The layer parameters. </returns>
        LayerParameters& GetLayerParameters() { return _layerParameters; }

        /// <summary> Returns the layer parameters. </summary>
        ///
        /// <returns> The layer parameters. </returns>
        const LayerParameters& GetLayerParameters() const { return _layerParameters; }

        /// <summary> Prints diagnostic info about the layer to the  output stream. </summary>
        ///
        /// <param name="os"> The stream that receives the formated output (e.g. std::out) </param>
        /// <param name="maxValuesToPrint"> The maximum number of values from the layer output to include in the info sent to the output stream </param>
        virtual void Print(std::ostream& os, size_t maxValuesToPrint = 100) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("Layer"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    protected:
        /// <summary> Returns a reference to the output tensor. </summary>
        ///
        /// <returns> Reference to the output tensor. </returns>
        TensorReferenceType GetOutputTensor() { return _output; }

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

    /// <summary> A serialization context used during layer deserialization. Wraps an existing `SerializationContext`
    /// and adds access to the layer being constructed. </summary>
    template <typename ElementType>
    class LayerSerializationContext : public utilities::SerializationContext
    {
        using ConstTensorReferenceType = typename Layer<ElementType>::ConstTensorReferenceType;

    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="previousContext"> The `SerializationContext` to wrap </param>
        LayerSerializationContext(utilities::SerializationContext& previousContext)
            : _previousContext(previousContext), _outputReference(math::Triplet{ 0, 0, 0 }) {}

        virtual ~LayerSerializationContext() {}

        /// <summary> Gets the type factory associated with this context. </summary>
        ///
        /// <returns> The type factory associated with this context. </returns>
        virtual utilities::GenericTypeFactory& GetTypeFactory() override { return _previousContext.GetTypeFactory(); }

        /// <summary> Sets the output reference to be saved in the context.
        ///
        /// <param name="outputReference"> The output reference to save, typically from the layer that has just been deserialized. </param>
        void SetOutputReference(ConstTensorReferenceType outputReference) { _outputReference = outputReference; }

        /// <summary> Returns the previously saved output reference. </summary>
        ///
        /// <returns> The outputReference stored in this context, typically from the previously deserialized layer. </returns>
        ConstTensorReferenceType GetPreviousOutputReference() { return _outputReference; }

    private:
        utilities::SerializationContext& _previousContext;
        ConstTensorReferenceType _outputReference;
    };
}
}
}

#include "../tcc/Layer.tcc"
