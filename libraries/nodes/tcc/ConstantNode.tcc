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
    // superclass (Node) constructor takes two array arguments: inputs and outputs. These are pointers to our local InputPort and OutputPort storage.

    // Default constructor
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode()
        : Node({}, { &_output }), _output(this, outputPortName, 1){};

    // Constructor for a scalar constant
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(ValueType value)
        : Node({}, { &_output }), _output(this, outputPortName, 1), _values({ value }){};

    // Constructor for a vector constant
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(const std::vector<ValueType>& values)
        : Node({}, { &_output }), _output(this, outputPortName, values.size()), _values(values){};

    template <typename ValueType>
    void ConstantNode<ValueType>::Compute() const
    {
        _output.SetOutput(_values);
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<ConstantNode<ValueType>>(_values);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[outputPortName] << _output;
        archiver["values"] << _values;
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[outputPortName] >> _output;
        archiver["values"] >> _values;
    }
}
