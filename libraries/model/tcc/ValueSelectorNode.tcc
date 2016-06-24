////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ValueSelectorNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{

    template <typename ValueType>
    ValueSelectorNode<ValueType>::ValueSelectorNode(const OutputRef<bool>& condition, const OutputRef<ValueType>& value1, const OutputRef<ValueType>& value2) : Node({ &_condition, &_value1, &_value2 }, { &_output }), _condition(this, 0, condition), _value1(this, 1, value1), _value2(this, 2, value2), _output(this, 0, value1.Size())
    {
        if (condition.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }

        if (value1.Size() != value2.Size())
        {
            throw std::runtime_error("Error: input values must be same dimension");
        }
    };

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::Compute() const
    {
        bool cond = _condition[0];
        _output.SetOutput(cond ? _value1.GetValue() : _value2.GetValue());
    };
}
