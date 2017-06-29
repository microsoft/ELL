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

// nodes
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
    /// <summary> Base class for neural network layer nodes. </summary
    template <typename ValueType>
    class NeuralNetworkLayerNodeBase : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

    protected:
        NeuralNetworkLayerNodeBase();
        NeuralNetworkLayerNodeBase(const model::PortElements<ValueType>& input, size_t outputSize);

        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;
    };

    /// <summary> Base class for neural network layer nodes. </summary
    template <typename DerivedType, typename LayerType, typename ValueType>
    class NeuralNetworkLayerNode : public NeuralNetworkLayerNodeBase<ValueType>
    {
    public:
        using NodeLayerType = LayerType;

        /// @name Input and Output Ports
        /// @{
        using NeuralNetworkLayerNodeBase<ValueType>::inputPortName; // "input"
        using NeuralNetworkLayerNodeBase<ValueType>::outputPortName; // "output"
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
        PortMemoryLayout& GetInputMemoryLayout() { return _inputLayout; }
        
        /// <summary> Gets information about the input memory layout </summary>
        const PortMemoryLayout& GetInputMemoryLayout() const { return _inputLayout; }
        
        /// <summary> Gets information about the output memory layout </summary>
        const PortMemoryLayout& GetOutputMemoryLayout() const { return _outputLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        PortMemoryLayout& GetOutputMemoryLayout() { return _outputLayout; }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    protected:
        size_t NumInputDimensions() const { return _inputLayout.size.size(); }
        virtual void Copy(model::ModelTransformer& transformer) const override;
        virtual void Compute() const override;
        using NeuralNetworkLayerNodeBase<ValueType>::_input;
        using NeuralNetworkLayerNodeBase<ValueType>::_output;

        mutable typename LayerType::TensorType _inputTensor;
        mutable LayerType _layer; // mutable to get around Compute being non-const

    private:
        PortMemoryLayout _inputLayout;
        PortMemoryLayout _outputLayout;
    };

    // helper:
    template <typename LayerType>
    typename LayerType::LayerParameters GetLayerNodeParameters(const typename LayerType::TensorType& inputTensor, const typename LayerType::LayerParameters& layerParameters);
}
}

#include "../tcc/NeuralNetworkLayerNode.tcc"
