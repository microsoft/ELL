////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    InputNode<ValueType>::InputNode(size_t dimension) : Node({}, { &_output }), _output(this, "output", dimension){};

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
    void InputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<InputNode<ValueType>>(_output.Size());
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void InputNode<ValueType>::Refine(ModelTransformer& transformer) const
    {
        auto newNode = transformer.AddNode<InputNode<ValueType>>(_output.Size());
         transformer.MapOutputPort(output, newNode->output);
    }

    /// Inherited from ISerializable
    template <typename ValueType>
    utilities::ObjectDescription InputNode<ValueType>::GetDescription() const
    {
        auto result = Node::GetDescription();
        result.AddField("size", utilities::MakeVariant<size_t>(_output.Size()));
        return result;
    }
}
