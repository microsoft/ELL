////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InputNodeBase.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "PortMemoryLayout.h"

#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeName.h>
#include <utilities/include/TypeTraits.h>

#include <string>
#include <vector>

namespace ell
{
namespace model
{
    class ModelTransformer;

    /// <summary> A node that represents an input to the system. </summary>
    template <typename ValueType>
    class InputNode : public InputNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Constructor </summary>
        InputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="size"> The input size </param>
        InputNode(size_t size);

        /// <summary> Constructor </summary>
        ///
        /// <param name="shape"> The input node's output shape </param>
        InputNode(MemoryShape shape);

        /// <summary> Constructor </summary>
        ///
        /// <param name="layout"> The input node's output memory layout </param>
        InputNode(const PortMemoryLayout& layout);

        /// <summary> Sets the value output by this node </summary>
        ///
        /// <param name="inputValues"> The value for this node to output </param>
        void SetInput(ValueType inputValue);

        /// <summary> Sets the value output by this node </summary>
        ///
        /// <param name="inputValues"> The values for this node to output </param>
        void SetInput(std::vector<ValueType> inputValues);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("InputNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary></summary>
        size_t Size() const { return _output.Size(); }

    protected:
        void Compute() const override;
        void Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Copy(ModelTransformer& transformer) const override;

        std::vector<ValueType> _inputValues;
        OutputPort<ValueType> _output;
    };

    /// <summary> Convenience function for adding a 1-dimensional InputNode to a model. </summary>
    ///
    /// <param name="model"> The Model or ModelTransformer to add the node to. </param>
    /// <param name="size"> The input node's size. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType, typename ModelLikeType, typename IntType, utilities::IsNonBooleanIntegral<IntType> concept = true>
    const OutputPort<ValueType>& Input(ModelLikeType& model, IntType size);

    /// <summary> Convenience function for adding an InputNode to a model. </summary>
    ///
    /// <param name="model"> The Model or ModelTransformer to add the node to. </param>
    /// <param name="layout"> The input node's output memory layout. </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType, typename ModelLikeType>
    const OutputPort<ValueType>& Input(ModelLikeType& model, const PortMemoryLayout& layout);
} // namespace model
} // namespace ell

#pragma region implementation

#include "ModelTransformer.h"

#include <utilities/include/StringUtil.h>

namespace ell
{
namespace model
{
    template <typename ValueType>
    InputNode<ValueType>::InputNode() :
        InputNodeBase(_output),
        _output(this, defaultOutputPortName, 0)
    {
        SetShape(MemoryShape{ 0 });
    }

    template <typename ValueType>
    InputNode<ValueType>::InputNode(size_t size) :
        InputNodeBase(_output),
        _output(this, defaultOutputPortName, size)
    {
        SetShape(MemoryShape{ static_cast<int>(size) });
    }

    template <typename ValueType>
    InputNode<ValueType>::InputNode(MemoryShape shape) :
        InputNodeBase(_output),
        _output(this, defaultOutputPortName, PortMemoryLayout{ shape })
    {
        SetShape(shape);
    }

    template <typename ValueType>
    InputNode<ValueType>::InputNode(const PortMemoryLayout& layout) :
        InputNodeBase(_output),
        _output(this, defaultOutputPortName, layout)
    {
        SetMemoryLayout(layout);
    }

    template <typename ValueType>
    void InputNode<ValueType>::SetInput(ValueType inputValue)
    {
        SetInput(std::vector<ValueType>{ inputValue });
    }

    template <typename ValueType>
    void InputNode<ValueType>::SetInput(std::vector<ValueType> inputValues)
    {
        if (_output.Size() != inputValues.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            ell::utilities::FormatString("InputNode output size %zu doesn't match input size %zu", _output.Size(), inputValues.size()));
        }
        _inputValues = inputValues;
    }

    template <typename ValueType>
    void InputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_inputValues);
    }

    template <typename ValueType>
    void InputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<InputNode<ValueType>>(GetShape());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void InputNode<ValueType>::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Input node is typically set up during pass1. By default, no further work needed
    }

    template <typename ValueType>
    void InputNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver["layout"] << _output.GetMemoryLayout();
    }

    template <typename ValueType>
    void InputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);

        int size;
        archiver.OptionalProperty("size", 0) >> size;
        std::vector<int> shapeVector;
        archiver.OptionalProperty("shape", std::vector<int>{ size }) >> shapeVector;
        if (archiver.HasNextPropertyName("layout"))
        {
            PortMemoryLayout layout;
            archiver["layout"] >> layout;
            SetShape(layout.GetActiveSize());
        }
        else
        {
            SetShape({ shapeVector });
        }
    }

    template <typename ValueType, typename ModelLikeType, typename IntType, utilities::IsNonBooleanIntegral<IntType> concept>
    const OutputPort<ValueType>& Input(ModelLikeType& model, IntType size)
    {
        static_assert(utilities::IsOneOf<ModelLikeType, model::Model, model::ModelTransformer>, "'model' parameter must be a model::Model or model::ModelTransformer");
        if constexpr (std::is_signed_v<IntType>)
        {
            if (size < 0)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidSize);
            }
        }

        auto node = model.template AddNode<InputNode<ValueType>>(static_cast<size_t>(size));
        return node->output;
    }

    template <typename ValueType, typename ModelLikeType>
    const OutputPort<ValueType>& Input(ModelLikeType& model, const PortMemoryLayout& layout)
    {
        static_assert(utilities::IsOneOf<ModelLikeType, model::Model, model::ModelTransformer>, "'model' parameter must be a model::Model or model::ModelTransformer");
        auto node = model.template AddNode<InputNode<ValueType>>(layout);
        return node->output;
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
