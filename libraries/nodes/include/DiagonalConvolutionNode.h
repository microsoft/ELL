////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DiagonalConvolutionNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/IRMapCompiler.h>
#include <model/include/ModelTransformer.h>
#include <model/include/PortElements.h>
#include <model/include/PortMemoryLayout.h>

#include <predictors/neural/include/ConvolutionalLayer.h>

#include <string>

namespace ell
{
namespace nodes
{
    /// <summary>
    /// If diagonal convolution is specified, a ConvolutionalLayerNode will refine
    /// itself into a DiagonalConvolutionNode.
    /// </summary>
    template <typename ValueType>
    class DiagonalConvolutionNode : public model::CompilableNode
    {
    public:
        using TensorType = typename predictors::neural::Layer<ValueType>::TensorType;
        using ConstTensorReferenceType = typename predictors::neural::Layer<ValueType>::ConstTensorReferenceType;

        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        DiagonalConvolutionNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        /// <param name="filterWeights"> The weights for the convolutional filters. Stored
        ///  as a 3D tensor of dimensions (nf*fw) x fw x d, where nf == # filters, fw == filter width, and d == input depth. </param>
        /// <param name="stride"> The output stride. </param>
        DiagonalConvolutionNode(const model::OutputPort<ValueType>& input,
                                const model::PortMemoryLayout& inputMemoryLayout,
                                const model::PortMemoryLayout& outputMemoryLayout,
                                const ConstTensorReferenceType& filterWeights,
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
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DiagonalConvolutionNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        void Compute() const override;
        bool Refine(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: convolutional parameters and memory layout

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;

        TensorType _filterWeights;

        int _stride = 1;
    };

    //
    // DiagonalConvolutionComputeNode
    //

    /// <summary>
    /// A node that does the actual convolution operation
    /// </summary>
    template <typename ValueType>
    class DiagonalConvolutionComputeNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* filterWeightsPortName = "filterWeights";
        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<ValueType>& filterWeights = _filterWeights;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        DiagonalConvolutionComputeNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="filterWeights"> The weights for the convolutional filters. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        /// <param name="filterSize"> The filter width. </param>
        /// <param name="stride"> The output stride. </param>
        DiagonalConvolutionComputeNode(const model::OutputPort<ValueType>& input,
                                       const model::OutputPort<ValueType>& filterWeights,
                                       const model::PortMemoryLayout& inputMemoryLayout,
                                       const model::PortMemoryLayout& outputMemoryLayout,
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
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DiagonalConvolutionComputeNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: convolutional parameters and memory layout

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Input
        model::InputPort<ValueType> _input;
        model::InputPort<ValueType> _filterWeights;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;

        int _filterSize = 0;
        int _stride = 1;
        int _batchSize = 0;
    };
} // namespace nodes
} // namespace ell
