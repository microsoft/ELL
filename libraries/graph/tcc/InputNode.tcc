//
// InputNode.tcc
//

template <typename ValueType>
InputNode<ValueType>::InputNode(size_t dimension) : Node({}, {&_output}), _output(this, 0, dimension) 
{
};
