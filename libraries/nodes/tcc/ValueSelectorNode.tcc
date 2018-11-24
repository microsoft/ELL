////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueSelectorNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
        const auto& newPortElements1 = transformer.GetCorrespondingInputs(_input1);
        const auto& newPortElements2 = transformer.GetCorrespondingInputs(_input2);

        auto newNode = transformer.AddNode<ValueSelectorNode<ValueType>>(newCondition, newPortElements1, newPortElements2);

        transformer.MapNodeOutput(output, newNode->output);
    }
} // namespace nodes
} // namespace ell
