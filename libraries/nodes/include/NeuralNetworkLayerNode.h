////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkLayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "CompilableNode.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"
#include "PortMemoryLayout.h"

// predictors
#include "Layer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> Parameters to influence how neural network layers behave when embedded as nodes in a graph </summary>
    struct NeuralNetworkLayerNodeParameters
    {
        bool includePaddingInInputData;
    };

    /// <summary> Base class for neural network layer nodes. </summary
    template <typename ValueType>
    class NeuralNetworkLayerNodeBase : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Gets information about the input memory layout </summary>
        virtual model::PortMemoryLayout& GetInputMemoryLayout() = 0;

        /// <summary> Gets information about the input memory layout </summary>
        virtual const model::PortMemoryLayout& GetInputMemoryLayout() const = 0;

        /// <summary> Gets information about the output memory layout </summary>
        virtual const model::PortMemoryLayout& GetOutputMemoryLayout() const = 0;

        /// <summary> Gets information about the output memory layout </summary>
        virtual model::PortMemoryLayout& GetOutputMemoryLayout() = 0;

        /// <summary> Gets the LayerParameters from the layer wrapped by this node </summary>
        virtual typename predictors::neural::Layer<ValueType>::LayerParameters GetLayerParameters() const = 0;

        /// <summary> Get the input padding requested by the layer </summary>
        predictors::neural::PaddingParameters GetRequestedInputPadding() const { return GetLayerParameters().inputPaddingParameters; }

        /// <summary> Get the output padding requested by the layer </summary>
        predictors::neural::PaddingParameters GetRequestedOutputPadding() const { return GetLayerParameters().inputPaddingParameters; }

        /// <summary> Get the size of the output port </summary>
        size_t GetOutputSize() const { return _output.Size(); }

    protected:
        NeuralNetworkLayerNodeBase();
        NeuralNetworkLayerNodeBase(const model::PortElements<ValueType>& input, const NeuralNetworkLayerNodeParameters& parameters, size_t outputSize);

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        NeuralNetworkLayerNodeParameters _parameters;
    };

    /// <summary> Base class for neural network layer nodes. </summary
    template <typename DerivedType, typename NodeLayerType, typename ValueType>
    class NeuralNetworkLayerNode : public NeuralNetworkLayerNodeBase<ValueType>
    {
    public:
        using LayerType = NodeLayerType;

        /// @name Input and Output Ports
        /// @{
        using NeuralNetworkLayerNodeBase<ValueType>::input;
        using NeuralNetworkLayerNodeBase<ValueType>::output;
        /// @}

        /// <summary> Default constructor. </summary>
        NeuralNetworkLayerNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input to the layer (typically the output of the previous layer). </param>
        /// <param name="layer"> The neural network layer to wrap. </param>
        NeuralNetworkLayerNode(const model::PortElements<ValueType>& input, const LayerType& layer);

        /// <summary> Gets the layer being wrapped </summary>
        const LayerType& GetLayer() const { return _layer; }

        /// <summary> Gets information about the input memory layout </summary>
        model::PortMemoryLayout& GetInputMemoryLayout() override { return _inputLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const override { return _inputLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        const model::PortMemoryLayout& GetOutputMemoryLayout() const override { return _outputLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        model::PortMemoryLayout& GetOutputMemoryLayout() override { return _outputLayout; }

        /// <summary> Gets the LayerParameters from the layer wrapped by this node </summary>
        typename predictors::neural::Layer<ValueType>::LayerParameters GetLayerParameters() const override { return _layer.GetLayerParameters(); }

    protected:
        size_t NumInputDimensions() const { return _inputLayout.NumDimensions(); }
        model::PortMemoryLayout CalculateMemoryLayout(size_t padding, typename predictors::neural::Layer<ValueType>::Shape dataBufferSize);
        void Copy(model::ModelTransformer& transformer) const override;
        void Compute() const override;
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        using NeuralNetworkLayerNodeBase<ValueType>::_input;
        using NeuralNetworkLayerNodeBase<ValueType>::_output;

        mutable typename LayerType::TensorType _inputTensor;
        mutable LayerType _layer; // mutable to get around Compute being non-const
        bool HasState() const override { return true; } // stored state: inputLayout, outputLayout

    private:
        model::PortMemoryLayout _inputLayout;
        model::PortMemoryLayout _outputLayout;
        math::TensorShape _inputShape;
    };

    // helper:
    template <typename LayerType>
    typename LayerType::LayerParameters GetLayerNodeParameters(const typename LayerType::TensorType& inputTensor, const typename LayerType::LayerParameters& layerParameters);
}
}

#include "../tcc/NeuralNetworkLayerNode.tcc"
