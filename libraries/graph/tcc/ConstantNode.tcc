////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstantNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor for a scalar constant

// superclass (Node) constructor takes two array arguments: inputs and outputs. These are pointers to our local InputPort and OutputPort storage.
template <typename ValueType>
ConstantNode<ValueType>::ConstantNode(ValueType value) : Node({}, {&_output}), _output(this, 0, 1), _values({value})
{
};

// Constructor for a vector constant
template <typename ValueType>
ConstantNode<ValueType>::ConstantNode(std::vector<ValueType> values) : Node({&_output}), _output(this, values.size()), _values(values)
{
};

template <typename ValueType>
void ConstantNode<ValueType>::Compute() const
{
    _output.SetOutput(_values);
}
