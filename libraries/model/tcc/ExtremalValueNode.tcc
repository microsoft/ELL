////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ExtremalValueNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType, bool max>
    ExtremalValueNode<ValueType, max>::ExtremalValueNode(const OutputPortElements<ValueType>& input) : Node({ &_input }, { &_val, &_argVal }), _input(this, input, inputPortName), _val(this, valPortName, 1), _argVal(this, argValPortName, 1)
    {
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::Compute() const
    {
        auto inputValues = _input.GetValue();
        decltype(std::max_element(inputValues.begin(), inputValues.end())) result;
        if (max)
        {
            result = std::max_element(inputValues.begin(), inputValues.end());
        }
        else
        {
            result = std::min_element(inputValues.begin(), inputValues.end());
        }
        auto val = *result;
        auto index = result - inputValues.begin();
        _val.SetOutput({ val });
        _argVal.SetOutput({ (int)index });
    };

    // 
    // Copy definitions for subclasses
    // 
    template <typename ValueType>
    void ArgMinNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<ArgMinNode<ValueType>>(newInput);
        transformer.MapOutputPort(val, newNode->val);
        transformer.MapOutputPort(argVal, newNode->argVal);
    }

    template <typename ValueType>
    void ArgMaxNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<ArgMaxNode<ValueType>>(newInput);
        transformer.MapOutputPort(val, newNode->val);
        transformer.MapOutputPort(argVal, newNode->argVal);
    }

}
