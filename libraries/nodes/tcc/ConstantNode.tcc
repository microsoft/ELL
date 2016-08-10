////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstantNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> nodes namespace </summary>
namespace nodes
{
    // Constructor for a scalar constant

    // superclass (Node) constructor takes two array arguments: inputs and outputs. These are pointers to our local InputPort and OutputPort storage.
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(ValueType value) : Node({}, { &_output }), _output(this, outputPortName, 1), _values({ value }) {};

    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode() : Node({}, { &_output }), _output(this, outputPortName, 1) {};

    // Constructor for a vector constant
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(const std::vector<ValueType>& values) : Node({}, { &_output }), _output(this, outputPortName, values.size()), _values(values){};

    template <typename ValueType>
    void ConstantNode<ValueType>::Compute() const
    {
        _output.SetOutput(_values);
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<ConstantNode<ValueType>>(_values);
         transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("values", _values);
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("values", _values, context);
    }
}
