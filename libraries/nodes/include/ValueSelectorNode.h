////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueSelectorNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/InputPort.h>
#include <model/include/Node.h>
#include <model/include/OutputPort.h>

#include <utilities/include/TypeName.h>

#include <exception>
#include <vector>

namespace ell
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> An example node that selects from one of two input values depending on a third input </summary>
    template <typename ValueType>
    class ValueSelectorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* conditionPortName = "condition";
        const model::InputPort<bool>& condition = _condition;
        const model::InputPort<ValueType>& input1 = _input1;
        const model::InputPort<ValueType>& input2 = _input2;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        ValueSelectorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="condition"> An input that returns a single boolean value that selects which input to use as output </param>
        /// <param name="input1"> The input to return if the condition is `true` </param>
        /// <param name="input2"> The input to return if the condition is `false` </param>
        ValueSelectorNode(const model::OutputPort<bool>& condition, const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ValueSelectorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return false; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<bool> _condition;
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<ValueType> _output;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    ValueSelectorNode<ValueType>::ValueSelectorNode() :
        Node({ &_condition, &_input1, &_input2 }, { &_output }),
        _condition(this, {}, conditionPortName),
        _input1(this, {}, defaultInput1PortName),
        _input2(this, {}, defaultInput2PortName),
        _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    ValueSelectorNode<ValueType>::ValueSelectorNode(const model::OutputPort<bool>& condition, const model::OutputPort<ValueType>& input1, const model::OutputPort<ValueType>& input2) :
        Node({ &_condition, &_input1, &_input2 }, { &_output }),
        _condition(this, condition, conditionPortName),
        _input1(this, input1, defaultInput1PortName),
        _input2(this, input2, defaultInput2PortName),
        _output(this, defaultOutputPortName, input1.Size())
    {
        if (condition.Size() != 1)
        {
            throw ell::utilities::Exception("Error: Condition must be 1-D signal");
        }

        if (input1.Size() != input2.Size())
        {
            throw ell::utilities::Exception("Error: input values must be same dimension");
        }
    };

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::Compute() const
    {
        bool cond = _condition[0];
        _output.SetOutput(cond ? _input1.GetValue() : _input2.GetValue());
    };

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInput1PortName] << _input1;
        archiver[defaultInput2PortName] << _input2;
        archiver[conditionPortName] << _condition;
    }

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInput1PortName] >> _input1;
        archiver[defaultInput2PortName] >> _input2;
        archiver[conditionPortName] >> _condition;
        _output.SetSize(_input1.Size());
    }

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newCondition = transformer.GetCorrespondingInputs(_condition);
        const auto& newInputs1 = transformer.GetCorrespondingInputs(_input1);
        const auto& newInputs2 = transformer.GetCorrespondingInputs(_input2);

        auto newNode = transformer.AddNode<ValueSelectorNode<ValueType>>(newCondition, newInputs1, newInputs2);

        transformer.MapNodeOutput(output, newNode->output);
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
