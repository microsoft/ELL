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
    auto inputValues = _input.GetValue<ValueType>();
    decltype(std::max_element(inputValues.begin(), inputValues.end())) result;    
    if(max)
    {
        result = std::max_element(inputValues.begin(), inputValues.end());
    }
    else
    {
        result = std::min_element(inputValues.begin(), inputValues.end());
    }    
    auto val = *result;
    auto index = result-inputValues.begin();
    _val.SetOutput({val});
    _argVal.SetOutput({(int)index});
};
