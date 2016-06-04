//
// ValueSelectorNode.tcc
//

template <typename ValueType>
ValueSelectorNode<ValueType>::ValueSelectorNode(NodeOutput<bool> condition, NodeOutput<ValueType> value1, NodeOutput<ValueType> value2): Node({&_condition, &_value1, &_value2}, {&_output}), _condition(condition), _value1(value1), _value2(value2), _output(this, 0, 1) 
{
};
