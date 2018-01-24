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
/// <summary> Macro to make adding enum entries with corresponding names more reliable
#define LAYER_TYPES_LIST \
    ADD_LAYER_TYPE(base, "Base")\
    ADD_LAYER_TYPE(activation, "Activation")\
    ADD_LAYER_TYPE(batchNormalization, "BatchNormalization")\
    ADD_LAYER_TYPE(bias, "Bias")\
    ADD_LAYER_TYPE(binaryConvolution, "BinaryConvolution")\
    ADD_LAYER_TYPE(convolution, "Convolution")\
    ADD_LAYER_TYPE(fullyConnected, "FullyConnected")\
    ADD_LAYER_TYPE(gru, "GRU")\
    ADD_LAYER_TYPE(input, "Input")\
    ADD_LAYER_TYPE(lstm, "LSTM")\
    ADD_LAYER_TYPE(pooling, "Pooling")\
    ADD_LAYER_TYPE(recurrent, "Recurrent")\
    ADD_LAYER_TYPE(region, "RegionDetection")\
    ADD_LAYER_TYPE(scaling, "Scaling")\
    ADD_LAYER_TYPE(softmax, "Softmax")


    /// <summary> Enum that represents the type of neural network layer. </summary>
    #define ADD_LAYER_TYPE(a, b) a,
    enum class LayerType : int
    {
        LAYER_TYPES_LIST
    };
    #undef ADD_LAYER_TYPE

    /// <summary> Vector of strings that contains the names of the neural network layer types. </summary>
    #define ADD_LAYER_TYPE(a, b) b,
    static const std::string LayerNames[] =
    {
        LAYER_TYPES_LIST
    };
    #undef ADD_LAYER_TYPE

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
    inline PaddingParameters NoPadding()
    {
        return { PaddingScheme::zeros, 0 };
    }

    /// <summary> Function to return parameters that represent padding the specified pixel width with zeros. </summary
    inline PaddingParameters ZeroPadding(size_t width)
    {
        return { PaddingScheme::zeros, width };
    }

    /// <summary> Function to return parameters that represent padding the specified pixel width with the minimum value. </summary
    inline const PaddingParameters MinPadding(size_t width)
    {
        return { PaddingScheme::min, width };
    }

    /// <summary> Function to return parameters that represent padding the specified pixel width with -1. </summary
    inline const PaddingParameters MinusOnePadding(size_t width)
    {
        return { PaddingScheme::minusOnes, width };
    }

    /// <summary> Helper function to determine if a PaddingParameters struct represents no padding </summary>
    inline bool HasPadding(const PaddingParameters& padding)
    {
        return padding.paddingSize != 0;
    }

    /// <summary> Helper function to determine if a PaddingParameters struct represents zero padding </summary>
    inline bool HasPadding(const PaddingParameters& padding, PaddingScheme scheme)
    {
        return HasPadding(padding) && (padding.paddingScheme == scheme);
    }

    /// <summary> Get the padding value to fill with </summary>
    template <typename ValueType>
    ValueType GetPaddingValue(PaddingScheme paddingScheme);

    /// <summary> Common base class for a layer in a neural network. </summary>
    template <typename ElementType>
    class Layer : public utilities::IArchivable
    {
    public:
        using Shape = math::TensorShape;

        using VectorType = math::ColumnVector<ElementType>;
        using VectorReferenceType = math::ColumnVectorReference<ElementType>;
        using ConstVectorReferenceType = math::ConstColumnVectorReference<ElementType>;

        using MatrixType = math::RowMatrix<ElementType>;
        using MatrixReferenceType = math::ConstMatrixReference<ElementType, math::MatrixLayout::rowMajor>;
        using ConstMatrixReferenceType = math::ConstMatrixReference<ElementType, math::MatrixLayout::rowMajor>;

        using TensorType = math::Tensor<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using TensorReferenceType = math::TensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using ConstTensorReferenceType = math::ConstTensorReference<ElementType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;

        using DataVectorType = data::DenseDataVector<ElementType>;

        /// <summary> Parameters common to all layers, specifying info related to input and output of the layer. </summary>
        struct LayerParameters
        {
            /// <summary> Reference to the input tensor. Its size includes the padding.</summary>
            ConstTensorReferenceType input;

            /// <summary> The padding requirements for the input. </summary>
            PaddingParameters inputPaddingParameters;

            /// <summary> The extents of the tensor in logical order (row, column, channel). This size includes padding. </summary>
            Shape outputShape;

            /// <summary> The padding requirements for the output. </summary>
            PaddingParameters outputPaddingParameters;
        };

        /// <summary> Initializes this class with the required information regarding inputs and outputs. </summary>
        ///
        /// <param name="layerParameters"> The parameters for this layer. </param>
        Layer(const LayerParameters& layerParameters);

        /// <summary> Instantiates a blank instance. Used for unarchiving purposes only. </summary>
        Layer()
            : _layerParameters{math::TensorShape{ 0, 0, 0 }, NoPadding(), { 0, 0, 0 }, NoPadding() }, _output{ 0, 0, 0 } {}

        /// <summary> Returns a reference to the input tensor. </summary>
        ///
        /// <returns> Reference to the output tensor. </returns>
        ConstTensorReferenceType GetInput() const { return _layerParameters.input; }

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
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        /// <summary> Returns a read reference to the sub tensor of the input that does not contain padding. </summary>
        ///
        /// <returns> Read reference to the input tensor, minus padding. </returns>
        ConstTensorReferenceType GetInputMinusPadding();

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
        LayerSerializationContext(utilities::SerializationContext& previousContext)
            : SerializationContext(previousContext, {}), _outputReference(math::IntegerTriplet{ 0, 0, 0 }) {}

        /// <summary> Sets the output reference to be saved in the context.
        ///
        /// <param name="outputReference"> The output reference to save, typically from the layer that has just been deserialized. </param>
        void SetOutputReference(ConstTensorReferenceType outputReference) { _outputReference = outputReference; } // STYLE discrepancy

        /// <summary> Returns the previously saved output reference. </summary>
        ///
        /// <returns> The outputReference stored in this context, typically from the previously deserialized layer. </returns>
        ConstTensorReferenceType GetPreviousOutputReference() { return _outputReference; }

    private:
        ConstTensorReferenceType _outputReference;
    };
}
}
}

#include "../tcc/Layer.tcc"
