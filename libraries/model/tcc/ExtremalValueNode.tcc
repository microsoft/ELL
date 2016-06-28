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
    void ExtremalValueNode<ValueType, max>::Refine(Model& newModel, std::unordered_map<const Node*, Node*>& nodeMap, std::unordered_map<const Port*, Port*>& portMap) const
    {
        auto newInputs = CopyInputPort(_input, portMap);

        auto newNode = newModel.AddNode<ExtremalValueNode<ValueType, max>>(newInputs);
        nodeMap[this] = newNode.get();

        portMap[&_val] = &(newNode->_val);
        portMap[&_argVal] = &(newNode->_argVal);
    }
}
