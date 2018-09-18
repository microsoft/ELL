////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnrolledConvolutionNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Tensor.h"

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"
#include "PortMemoryLayout.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that implements convolution using matrix multiply on a reshaped input image. </summary>
    /// If Unrolled convolution is specified, a ConvolutionalLayerNode will refine
    /// itself into a UnrolledConvolutionNode.
    template <typename ValueType>
    class UnrolledConvolutionNode : public model::CompilableNode
    {
    public:
        using MatrixType = math::RowMatrix<ValueType>;
        using ConstMatrixReferenceType = math::ConstRowMatrixReference<ValueType>;
        using TensorType = math::ChannelColumnRowTensor<ValueType>;
        using ConstTensorReferenceType = math::ConstChannelColumnRowTensorReference<ValueType>;

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
        UnrolledConvolutionNode(const model::OutputPort<ValueType>& input,
                                const model::PortMemoryLayout& inputMemoryLayout,
                                const model::PortMemoryLayout& outputMemoryLayout,
                                const ConstTensorReferenceType& filterWeights,
                                int stride);

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="filterWeights"> The weights for the convolutional filters, expressed as a matrix. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        UnrolledConvolutionNode(const model::OutputPort<ValueType>& input,
                                const model::PortMemoryLayout& inputMemoryLayout,
                                const model::PortMemoryLayout& outputMemoryLayout,
                                ConstMatrixReferenceType filterWeights,
                                int filterSize,
                                int stride);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        model::PortMemoryLayout GetOutputMemoryLayout() const { return _output.GetMemoryLayout(); }

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return GetInputMemoryLayout().GetLogicalDimensionOrder() == order;
        }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("UnrolledConvolutionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return _isDepthwiseSeparable; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: convolutional parameters and memory layout

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        MatrixType GetWeightsMatrix(const ConstTensorReferenceType& weightsTensor) const;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;

        MatrixType _filterWeights;

        int _filterSize = 0;
        int _stride = 1;
        bool _isDepthwiseSeparable = false;
    };
}
}
