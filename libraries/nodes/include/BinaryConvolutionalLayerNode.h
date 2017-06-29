////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryConvolutionalLayerNode.h (nodes)
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
        using BaseType::inputPortName; // "input"
        using BaseType::outputPortName; // "output"
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
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        virtual bool IsCompilable() const override { return false; }

    protected:
        virtual bool Refine(model::ModelTransformer& transformer) const override;
        virtual bool HasState() const override { return true; }

    private:
        std::vector<int64_t> GetCompressedFilterWeights() const;
        std::vector<ValueType> GetFilterMeans() const;
    };

    //
    // BinarizeAndReshapeImageNode
    //
    template <typename ValueType, typename PackedBitsType = int64_t>
    class BinarizeAndReshapeImageNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<PackedBitsType>& output = _output;
        /// @}

        BinarizeAndReshapeImageNode();
        BinarizeAndReshapeImageNode(const model::PortElements<ValueType>& input,
                                    const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                                    const PortMemoryLayout& inputMemoryLayout,
                                    const PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets information about the input memory layout </summary>
        const PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }
        PortMemoryLayout& GetInputMemoryLayout() { return _inputMemoryLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        const PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }
        PortMemoryLayout& GetOutputMemoryLayout() { return _outputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, PackedBitsType>("BinarizeAndReshapeImageNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void Copy(model::ModelTransformer& transformer) const override;
        void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual bool HasState() const override { return false; }

        virtual void WriteToArchive(utilities::Archiver& archiver) const override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

    private:
        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<PackedBitsType> _output;

        predictors::neural::BinaryConvolutionalParameters _convolutionalParameters;
        PortMemoryLayout _inputMemoryLayout;
        PortMemoryLayout _outputMemoryLayout;
    };

    //
    // BinaryXnorNode
    //
    template <typename ValueType, typename PackedBitsType = int64_t>
    class BinaryXnorNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* filterWeightsPortName = "filterWeights";
        static constexpr const char* filterMeansPortName = "filterMeans";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<PackedBitsType>& input = _input;
        const model::InputPort<PackedBitsType>& filterWeights = _filterWeights;
        const model::InputPort<ValueType>& filterMeans = _filterMeans;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default contructor. </summary>
        BinaryXnorNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The ports to get input data from. </param>
        /// <param name="filterWeights"> The packed binary weights for the convolutional filters. </param>
        /// <param name="filterMeans"> The real-valued means of the convolutional filters. </param>
        /// <param name="convolutionalParameters"> The convolutional parameters. </param>
        /// <param name="inputMemoryLayout"> The layout of the input data. </param>
        /// <param name="outputMemoryLayout"> The layout of the output data. </param>
        BinaryXnorNode(const model::PortElements<PackedBitsType>& input,
                       const model::PortElements<PackedBitsType>& filterWeights,
                       const model::PortElements<ValueType>& filterMeans,
                       const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                       const PortMemoryLayout& inputMemoryLayout,
                       const PortMemoryLayout& outputMemoryLayout);

        /// <summary> Gets information about the input memory layout </summary>
        const PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        const PortMemoryLayout& GetOutputMemoryLayout() const { return _outputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, PackedBitsType>("BinaryXnorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void Copy(model::ModelTransformer& transformer) const override;
        void Compute() const override;
        virtual void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        virtual bool HasState() const override { return true; }

        virtual void WriteToArchive(utilities::Archiver& archiver) const override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

    private:
        // Input
        model::InputPort<PackedBitsType> _input;
        model::InputPort<PackedBitsType> _filterWeights;
        model::InputPort<ValueType> _filterMeans;

        // Output
        model::OutputPort<ValueType> _output;

        predictors::neural::BinaryConvolutionalParameters _convolutionalParameters;
        PortMemoryLayout _inputMemoryLayout;
        PortMemoryLayout _outputMemoryLayout;
    };
}
}
