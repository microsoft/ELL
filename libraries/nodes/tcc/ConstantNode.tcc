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
    ConstantNode<ValueType>::ConstantNode() : Node({}, { &_output }), _output(this, outputPortName, 1) {};

    // Constructor for a scalar constant
    template <typename ValueType>
    ConstantNode<ValueType>::ConstantNode(ValueType value) : Node({}, { &_output }), _output(this, outputPortName, 1), _values({ value })
    {};

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
         transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    utilities::ObjectDescription ConstantNode<ValueType>::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<Node, ConstantNode<ValueType>>("Constant node");
        description.template AddProperty<decltype(_values)>("values", "Constant values");
        description.template AddProperty<decltype(_output)>("output", "Output port");
        return description;
    }

    template <typename ValueType>
    utilities::ObjectDescription ConstantNode<ValueType>::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Node, ConstantNode<ValueType>>();
        description["output"] = _output;
        description["values"] = _values;
        return description;
    }

    template <typename ValueType>
    void ConstantNode<ValueType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description["values"] >> _values;
        description["output"] >> _output;
    }
}
