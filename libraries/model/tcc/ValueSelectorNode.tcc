////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ValueSelectorNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    ValueSelectorNode<ValueType>::ValueSelectorNode() : Node({ &_condition, &_input1, &_input2 }, { &_output }), _condition(this, {}, conditionPortName), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    ValueSelectorNode<ValueType>::ValueSelectorNode(const PortElements<bool>& condition, const PortElements<ValueType>& input1, const PortElements<ValueType>& input2) : Node({ &_condition, &_input1, &_input2 }, { &_output }), _condition(this, condition, conditionPortName), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, input1.Size())
    {
        if (condition.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }

        if (input1.Size() != input2.Size())
        {
            throw std::runtime_error("Error: input values must be same dimension");
        }
    };

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::Compute() const
    {
        bool cond = _condition[0];
        _output.SetOutput(cond ? _input1.GetValue() : _input2.GetValue());
    };

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("condition", _condition);
        serializer.Serialize("input1", _input1);
        serializer.Serialize("input2", _input2);
        serializer.Serialize("output", _output);
    }

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("condition", _condition, context);
        serializer.Deserialize("input1", _input1, context);
        serializer.Deserialize("input2", _input2, context);
        serializer.Deserialize("output", _output, context);
    }

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newCondition = transformer.TransformPortElements(_condition.GetPortElements());
        auto newPortElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto newPortElements2 = transformer.TransformPortElements(_input2.GetPortElements());

        auto newNode = transformer.AddNode<ValueSelectorNode<ValueType>>(newCondition, newPortElements1, newPortElements2);

         transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool ValueSelectorNode<ValueType>::Refine(ModelTransformer& transformer) const
    {
        auto newCondition = transformer.TransformPortElements(_condition.GetPortElements());
        auto newPortElements1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto newPortElements2 = transformer.TransformPortElements(_input2.GetPortElements());

        auto newNode = transformer.AddNode<ValueSelectorNode<ValueType>>(newCondition, newPortElements1, newPortElements2);

        transformer.MapNodeOutput(output, newNode->output);
        return true;
    }
}
