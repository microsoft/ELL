//
// InputNode.tcc
//

template <typename ValueType>
InputNode<ValueType>::InputNode(size_t dimension) : Node({}, {&_output}), _output(this, 0, dimension) 
{
};

template <typename ValueType>
void InputNode<ValueType>::SetInput(std::vector<ValueType> inputValues)
{
    _output.SetOutput(inputValues);
}