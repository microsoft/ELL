//
// ExtremalValueNode.tcc
//

template <typename ValueType, bool max>
ExtremalValueNode<ValueType, max>::ExtremalValueNode(NodeOutput<ValueType> input) : Node({&_input}, {&_val, &_argVal}), _input(input), _val(this, 0, 1), _argVal(this, 1, 1) 
{
};

template <typename ValueType, bool max>
void ExtremalValueNode<ValueType, max>::ComputeOutput() const 
{
    auto values = _input.GetValue<ValueType>();
    // TODO: compute stuff here
};
