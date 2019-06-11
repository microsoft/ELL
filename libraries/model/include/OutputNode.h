////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelTransformer.h"
#include "OutputNodeBase.h"
#include "OutputPort.h"

#include <utilities/include/IArchivable.h>
#include <utilities/include/TypeTraits.h>

#include <memory>
#include <string>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{

    /// <summary> A node that represents an output from the system. </summary>
    template <typename ValueType>
    class OutputNode : public OutputNodeBase
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* shapeName = "shape";
        const InputPort<ValueType>& input = _input;
        const OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        OutputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The port to get the input data from </param>
        OutputNode(const OutputPort<ValueType>& input);

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The port to get the input data from </param>
        /// <param name="shape"> The shape of the output data </param>
        OutputNode(const OutputPort<ValueType>& input, const MemoryShape& shape);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("OutputNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        void Compute() const override;
        InputPort<ValueType> _input;
        OutputPort<ValueType> _output;

    private:
        void Copy(ModelTransformer& transformer) const override;
    };

    /// <summary> Convenience function for adding an OutputNode to a model. </summary>
    ///
    /// <param name="input"> The port to get the input data from </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType>
    const OutputPort<ValueType>& Output(const OutputPort<ValueType>& input);

    /// <summary> Convenience function for adding an OutputNode to a model. </summary>
    ///
    /// <param name="input"> The port to get the input data from </param>
    /// <param name="shape"> The shape of the output data </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType>
    const OutputPort<ValueType>& Output(const OutputPort<ValueType>& input, const MemoryShape& shape);
} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    template <typename ValueType>
    OutputNode<ValueType>::OutputNode() :
        OutputNodeBase(_input, _output, {}),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {
        SetShape({});
    }

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const OutputPort<ValueType>& input) :
        OutputNodeBase(_input, _output, MemoryShape{ static_cast<int>(input.Size()) }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, input.Size())
    {
        SetShape(MemoryShape{ static_cast<int>(input.Size()) });
    }

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const OutputPort<ValueType>& input, const MemoryShape& shape) :
        OutputNodeBase(_input, _output, shape),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, input.Size())
    {
        SetShape(shape);
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<OutputNode<ValueType>>(newInputs, GetShape());
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void OutputNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["layout"] << _input.GetMemoryLayout();
    }

    template <typename ValueType>
    void OutputNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        PortMemoryLayout layout;
        archiver["layout"] >> layout;
        SetShape(layout.GetActiveSize());
    }

    template <typename ValueType>
    const OutputPort<ValueType>& Output(const OutputPort<ValueType>& input)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }
        auto node = model->AddNode<OutputNode<ValueType>>(input);
        return node->output;
    }

    template <typename ValueType>
    const OutputPort<ValueType>& Output(const OutputPort<ValueType>& input, const MemoryShape& shape)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }
        auto node = model->AddNode<OutputNode<ValueType>>(input, shape);
        return node->output;
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
