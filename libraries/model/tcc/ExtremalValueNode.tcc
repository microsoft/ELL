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
    ExtremalValueNode<ValueType, max>::ExtremalValueNode(const OutputPortElementList<ValueType>& input) : Node({ &_input }, { &_val, &_argVal }), _input(this, input), _val(this, 1), _argVal(this, 1)
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

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::CopyImpl(ModelTransformer& transformer) const
    {
        auto newInputs = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<ExtremalValueNode<ValueType, max>>(newInputs);
        transformer.MapAllOutputPorts(this, newNode);
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::RefineImpl(ModelTransformer& transformer) const
    {
        auto newInputs = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<ExtremalValueNode<ValueType, max>>(newInputs);
        transformer.MapAllOutputPorts(this, newNode);
    }
}
