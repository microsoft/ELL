////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SumNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    SumNode<ValueType>::SumNode() : Node({&_input}, {&_output}), _input(this, {}, inputPortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    SumNode<ValueType>::SumNode(const model::PortElements<ValueType>& input) : Node({&_input}, {&_output}), _input(this, input, inputPortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    void SumNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for(size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += v;
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void SumNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SumNode<ValueType>>(newPortElements);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void SumNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("input", _input);
        serializer.Serialize("output", _output);
    }

    template <typename ValueType>
    void SumNode<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("input", _input, context);
        serializer.Deserialize("output", _output, context);
    }
}
