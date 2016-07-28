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
    ConstantNode<ValueType>::ConstantNode(ValueType value) : Node({}, { &_output }), _output(this, outputPortName, 1), _values({ value }){};

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

    /// Inherited from ISerializable
    // template <typename ValueType>
    // utilities::ObjectDescription ConstantNode<ValueType>::GetDescription() const
    // {
    //     std::cout << "Serializing ConstantNode" << std::endl;
    //     auto result = Node::GetDescription();
    //     result.AddField("values", _values);
    //     return result;
    // }

    template <typename ValueType>
    void ConstantNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        std::cout << "Serializing ConstantNode" << std::endl;
        Node::Serialize(serializer);
        serializer.Serialize("values", _values);
    }
}
