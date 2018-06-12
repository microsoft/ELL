////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WinogradConvolutionNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dsp
#include "WinogradConvolution.h"

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
    /// <summary> A node that implements convolution using Winograd convolution. </summary>
    /// If Winograd convolution is specified, a ConvolutionalLayerNode will refine
    /// itself into a WinogradConvolutionNode.
    template <typename ValueType>
    class WinogradConvolutionNode : public model::CompilableNode
    {
    public:
        using TensorType = math::Tensor<ValueType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using ConstTensorReferenceType = math::ConstTensorReference<ValueType, math::Dimension::channel, math::Dimension::column, math::Dimension::row>;
        using FilterOrder = dsp::WinogradFilterOrder;

        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        WinogradConvolutionNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The port to get input data from. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="filterWeights"> The weights for the convolutional filters. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        /// <param name="stride"> The number of elements to move/jump when sliding over the input. Typically this is 1 to 3. </param>
        /// <param name="tileSize"> The size of the output tiles --- the number of output values to produce at a time. </param>
        /// <param name="order"> The order to process data during convolution. </param>
        WinogradConvolutionNode(const model::PortElements<ValueType>& input,
                                const model::PortMemoryLayout& inputMemoryLayout,
                                const model::PortMemoryLayout& outputMemoryLayout,
                                const ConstTensorReferenceType& filterWeights,
                                int stride,
                                int tileSize,
                                FilterOrder order);

        /// <summary> Cloning constructor </summary>
        ///
        /// <param name="other"> The node to copy configuration from. </param>
        /// <param name="input"> The port to get input data from. </param>
        WinogradConvolutionNode(const WinogradConvolutionNode<ValueType>& other, const model::PortElements<ValueType>& input);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("WinogradConvolutionNode"); }

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
        void Compute() const override;
        bool Refine(model::ModelTransformer& transformer) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: convolutional parameters and memory layout

    private:
        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;

        TensorType _filterWeights;

        int _stride = 1;
        int _tileSize = 0;
        int _filterSize = 0;
        FilterOrder _order = FilterOrder::tilesFirst;        
    };

    //
    // WinogradConvolutionComputeNode
    //

    /// <summary>
    /// A node that does the actual convolution operation
    /// </summary>
    template <typename ValueType>
    class WinogradConvolutionComputeNode : public model::CompilableNode
    {
    public:
        using FilterOrder = dsp::WinogradFilterOrder;

        /// @name Input and Output Ports
        /// @{
        static constexpr const char* filterWeightsPortName = "filterWeights";
        const model::InputPort<ValueType>& input = _input;
        const model::InputPort<ValueType>& filterWeights = _filterWeights;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        WinogradConvolutionComputeNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="filterWeights"> The weights for the convolutional filters. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        /// <param name="order"> The order to process data during convolution. </param>
        WinogradConvolutionComputeNode(const model::PortElements<ValueType>& input,
                                       const model::PortElements<ValueType>& filterWeights,
                                       const model::PortMemoryLayout& inputMemoryLayout,
                                       const model::PortMemoryLayout& outputMemoryLayout,
                                       int stride,
                                       int tileSize,
                                       int filterSize,
                                       FilterOrder order,
                                       int numFilterChannels);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("WinogradConvolutionComputeNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node into the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` object currently creating a new model </param>
        void Copy(model::ModelTransformer& transformer) const override;

        // Cloning constructor
        WinogradConvolutionComputeNode(const WinogradConvolutionComputeNode<ValueType>& other,
                                                                              const model::PortElements<ValueType>& input,
                                                                              const model::PortElements<ValueType>& filterWeights);
    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
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
        void CompileFiltersFirst(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* input, llvm::Value* transformedFilters, llvm::Value* output);
        void CompileTilesFirst(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function, llvm::Value* input, llvm::Value* transformedFilters, llvm::Value* output);

        // Input
        model::InputPort<ValueType> _input;
        model::InputPort<ValueType> _filterWeights;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;

        int n_numFilters = 0;
        int _stride = 1;

        // Winograd-specific parameters
        int _tileSize = 0;
        int _filterSize = 0;
        FilterOrder _order = FilterOrder::tilesFirst;        
        int _numFilterChannels = 0;

        // Tunable parameters
        int _inputBlockSize = 1;
        int _outputBlockSize = 1;
    };
}
}
