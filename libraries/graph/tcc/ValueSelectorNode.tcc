////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ValueSelectorNode.tcc (graph)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename ValueType>
ValueSelectorNode<ValueType>::ValueSelectorNode(OutputPort<bool> condition, OutputPort<ValueType> value1, OutputPort<ValueType> value2): Node({&_condition, &_value1, &_value2}, {&_output}), _condition(&condition), _value1(&value1), _value2(&value2), _output(this, 0, value1.Size()) 
{
    if(condition.Size() != 1)
    {
        throw std::runtime_error("Error: Condition must be 1-D signal");
    }
    if(value1.Size() != value2.Size())
    {
        throw std::runtime_error("Error: input values must be same dimension");
    }
};

template <typename ValueType>
void ValueSelectorNode<ValueType>::Compute() const 
{
    auto cond = _condition.GetValue<bool>()[0]; // array of size 1
    auto val1 = _value1.GetValue<ValueType>();
    auto val2 = _value2.GetValue<ValueType>();

    _output.SetOutput(cond ? val1 : val2);
};
