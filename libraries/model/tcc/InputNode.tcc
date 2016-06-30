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
    void InputNode<ValueType>::CopyImpl(ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<InputNode<ValueType>>(_output.Size());
        MapAllOutputPorts(transformer, newNode);
    }

    template <typename ValueType>
    void InputNode<ValueType>::RefineImpl(ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<InputNode<ValueType>>(_output.Size());
        MapAllOutputPorts(transformer, newNode);
    }
}
