////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnrolledConvolutionNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NeuralNetworkLayerNode.h"

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"

// predictors
#include "ConvolutionalLayer.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that implements convolution using matrix multiply on a reshaped input image. </summary>
    template <typename ValueType>
    class UnrolledConvolutionNode : public model::CompilableNode
    {
    public:
        using MatrixType = typename predictors::neural::Layer<ValueType>::MatrixType;
        using ConstMatrixReferenceType = typename predictors::neural::Layer<ValueType>::ConstMatrixReferenceType;
        using TensorType = typename predictors::neural::Layer<ValueType>::TensorType;
        using ConstTensorReferenceType = typename predictors::neural::Layer<ValueType>::ConstTensorReferenceType;

        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        UnrolledConvolutionNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="filterWeights"> The weights for the convolutional filters. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        /// <param name="convolutionalParameters"> The convolutional parameters. </param>
        UnrolledConvolutionNode(const model::PortElements<ValueType>& input,
                                const model::PortMemoryLayout& inputMemoryLayout,
                                const model::PortMemoryLayout& outputMemoryLayout,
                                const ConstTensorReferenceType& filterWeights,
                                const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                                const predictors::neural::PaddingParameters& inputPaddingParameters,
                                const predictors::neural::PaddingParameters& outputPaddingParameters);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="filterWeights"> The weights for the convolutional filters, expressed as a matrix. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        /// <param name="convolutionalParameters"> The convolutional parameters. </param>
        UnrolledConvolutionNode(const model::PortElements<ValueType>& input,
                                const model::PortMemoryLayout& inputMemoryLayout,
                                const model::PortMemoryLayout& outputMemoryLayout,
                                ConstMatrixReferenceType filterWeights,
                                const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                                const predictors::neural::PaddingParameters& inputPaddingParameters,
                                const predictors::neural::PaddingParameters& outputPaddingParameters);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        /// <summary> Get the parameters used to control convolution. </summary>
        ///
        /// <returns> A ConvolutionalParameters struct. </returns>
        const predictors::neural::ConvolutionalParameters& GetConvolutionalParameters() const { return _convolutionalParameters; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("UnrolledConvolutionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node into the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` object currently creating a new model </param>
        void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;
        void Compute() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        void ReadFromArchive(utilities::Unarchiver& archiver) override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
        bool HasState() const override { return true; } // stored state: convolutional parameters and memory layout

    private:
        MatrixType GetWeightsMatrix(const ConstTensorReferenceType& weightsTensor) const;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;

        MatrixType _filterWeights;

        predictors::neural::ConvolutionalParameters _convolutionalParameters;
        predictors::neural::PaddingParameters _inputPaddingParameters;
        predictors::neural::PaddingParameters _outputPaddingParameters;

        size_t _numWeightsRows = 0;
        size_t _numWeightsColumns = 0;
    };
}
}
