////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    InputNode<ValueType>::InputNode(size_t dimension) : Node({}, { &_output }), _output(this, dimension){};

    template <typename ValueType>
    void InputNode<ValueType>::SetInput(std::vector<ValueType> inputValues)
    {
        _inputValues = inputValues;
    }

    template <typename ValueType>
    void InputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_inputValues);
    }

    template <typename ValueType>
    void InputNode<ValueType>::Copy(Model& newModel, std::unordered_map<const Node*, Node*>& nodeMap, std::unordered_map<const Port*, Port*>& portMap) const
    {
        auto newNode = newModel.AddNode<InputNode<ValueType>>(_output.Size());
        nodeMap[this] = newNode.get();

        portMap[&_output] = &(newNode->_output);
    }

    template <typename ValueType>
    void InputNode<ValueType>::Refine(Model& newModel, std::unordered_map<const Node*, Node*>& nodeMap, std::unordered_map<const Port*, Port*>& portMap) const
    {
        auto newNode = newModel.AddNode<InputNode<ValueType>>(_output.Size());
        nodeMap[this] = newNode.get();

        portMap[&_output] = &(newNode->_output);
    }
}
