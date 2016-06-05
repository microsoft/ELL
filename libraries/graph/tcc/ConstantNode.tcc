//
// ConstantNode.tcc
//

// Constructor for a scalar constant

// superclass (Node) constructor takes two array arguments: inputs and outputs. These are pointers to our local NodeInput and NodeOutput storage.
// I know, kind of gross. 
template <typename ValueType>
ConstantNode<ValueType>::ConstantNode(ValueType value) : Node({}, {&_output}), _output(this, 0, 1), _values({value})
{
    _output.SetOutput(_values);
};

// Constructor for a vector constant
template <typename ValueType>
ConstantNode<ValueType>::ConstantNode(std::vector<ValueType> values) : Node({&_output}), _output(this, values.size()), _values(values)
{
    _output.SetOutput(_values);
};
