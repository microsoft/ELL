////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryConvolutionalLayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "NeuralNetworkLayerNode.h"
#include "PortElements.h"

// predictors
#include "BinaryConvolutionalLayer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> A node that wraps a neural net BinaryConvolutionalLayer. </summary>
    template <typename ValueType>
    class BinaryConvolutionalLayerNode : public NeuralNetworkLayerNode<BinaryConvolutionalLayerNode<ValueType>, predictors::neural::BinaryConvolutionalLayer<ValueType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::BinaryConvolutionalLayer<ValueType>;
        using BaseType = NeuralNetworkLayerNode<BinaryConvolutionalLayerNode<ValueType>, predictors::neural::BinaryConvolutionalLayer<ValueType>, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::input;
        using BaseType::output;
        /// @}

        BinaryConvolutionalLayerNode() = default;

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        BinaryConvolutionalLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::BinaryConvolutionalLayer<ValueType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BinaryConvolutionalLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;

    private:
        template <typename PackedBitsType>
        std::vector<PackedBitsType> GetCompressedFilterWeights() const;

        std::vector<ValueType> GetFilterMeans() const;

        template <typename PackedBitsType>
        std::vector<PackedBitsType> GetCompressedInputPaddingMask() const;

        std::vector<int> GetInputPaddingMaskSums() const;

        template <typename PackedBitsType>
        model::PortElements<ValueType> AddRefinedNodes(model::ModelTransformer& transformer, const model::PortElements<ValueType>& input) const;
    };

    //
    // BinaryReceptiveFieldMatrixNode
    //
    template <typename ValueType, typename PackedBitsType>
    class BinaryReceptiveFieldMatrixNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<PackedBitsType>& output = _output;
        /// @}

        /// <summary></summary>
        BinaryReceptiveFieldMatrixNode();

        /// <summary></summary>
        BinaryReceptiveFieldMatrixNode(const model::PortElements<ValueType>& input,
                                       const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                                       const model::PortMemoryLayout& inputMemoryLayout,
                                       const model::PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary></summary>
        model::PortMemoryLayout& GetInputMemoryLayout() { return _inputMemoryLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        const model::PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        /// <summary></summary>
        model::PortMemoryLayout& GetOutputMemoryLayout() { return _outputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, PackedBitsType>("BinaryReceptiveFieldMatrixNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Copy(model::ModelTransformer& transformer) const override;
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        bool HasState() const override { return false; }
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        emitters::IRFunctionEmitter GetTaskFunction(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<PackedBitsType> _output;

        predictors::neural::BinaryConvolutionalParameters _convolutionalParameters;
        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;
    };

    //
    // BinaryXnorNode
    //
    template <typename ValueType, typename PackedBitsType>
    class BinaryXnorNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPaddingMasksPortName = "inputPaddingMasks";
        static constexpr const char* inputPaddingMaskSumsPortName = "inputPaddingMaskSums";
        static constexpr const char* filterWeightsPortName = "filterWeights";
        static constexpr const char* filterMeansPortName = "filterMeans";
        const model::InputPort<PackedBitsType>& input = _input;
        const model::InputPort<PackedBitsType>& inputPaddingMasks = _inputPaddingMasks;
        const model::InputPort<int>& inputPaddingMaskSums = _inputPaddingMaskSums;
        const model::InputPort<PackedBitsType>& filterWeights = _filterWeights;
        const model::InputPort<ValueType>& filterMeans = _filterMeans;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default contructor. </summary>
        BinaryXnorNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The image data after being expanded into a GEMM-friendly order, binarized, and packed (via the BinaryReceptiveFieldMatrixNode). </param>
        /// <param name="inputPaddingMasks"> The packed padding masks for the input data. </param>
        /// <param name="inputPaddingMaskSums"> The sum of padding pixels per row of the shaped input data. </param>
        /// <param name="filterWeights"> The packed binary weights for the convolutional filters. </param>
        /// <param name="filterMeans"> The real-valued means of the convolutional filters. </param>
        /// <param name="convolutionalParameters"> The convolutional parameters. </param>
        /// <param name="inputPaddingParameters"> The input padding parameters. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        BinaryXnorNode(const model::PortElements<PackedBitsType>& input,
                       const model::PortElements<PackedBitsType>& inputPaddingMasks,
                       const model::PortElements<int>& inputPaddingMaskSums,
                       const model::PortElements<PackedBitsType>& filterWeights,
                       const model::PortElements<ValueType>& filterMeans,
                       const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                       const predictors::neural::PaddingParameters& inputPaddingParameters,
                       const model::PortMemoryLayout& inputMemoryLayout,
                       const model::PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets information about the input memory layout of the original BinaryConvolutionalLayoutNode </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        const model::PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, PackedBitsType>("BinaryXnorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Copy(model::ModelTransformer& transformer) const override;
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void ComputeFilterOutput(model::IRMapCompiler& compiler,
                                 emitters::IRFunctionEmitter& function,
                                 llvm::Value* pInput,
                                 llvm::Value* pFilterWeights,
                                 llvm::Value* pFilterMeans,
                                 llvm::Value* pInputPaddingMask,
                                 llvm::Value* pInputPaddingMaskSums,
                                 llvm::Value* pOutput,
                                 llvm::Value* filterIndex,
                                 bool hasZeroPadding,
                                 int outputColumns,
                                 int packedRowSize,
                                 int packedRowStride,
                                 bool useVectorInstructions,
                                 int vectorSize,
                                 int numVectorBlocks);

        bool HasState() const override { return true; } // stored state: convolutional parameters and input/output memory layouts
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void EmitInnerLoop(emitters::IRFunctionEmitter& function,
                           llvm::Value* reshapedInput,
                           llvm::Value* paddingMask,
                           llvm::Value* weights,
                           llvm::Value* xorSumVariable,
                           llvm::Function* popCountFunction,
                           int startBlock,
                           int numBlocks,
                           bool hasZeroPadding);

        emitters::IRFunctionEmitter GetTaskFunction(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function);

        // Input
        model::InputPort<PackedBitsType> _input;
        model::InputPort<PackedBitsType> _inputPaddingMasks;
        model::InputPort<int> _inputPaddingMaskSums;
        model::InputPort<PackedBitsType> _filterWeights;
        model::InputPort<ValueType> _filterMeans;

        // Output
        model::OutputPort<ValueType> _output;

        predictors::neural::BinaryConvolutionalParameters _convolutionalParameters;
        predictors::neural::PaddingParameters _inputPaddingParameters;
        model::PortMemoryLayout _inputMemoryLayout;
        model::PortMemoryLayout _outputMemoryLayout;
    };
}
}
