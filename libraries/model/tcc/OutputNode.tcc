////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::OutputPortElements<ValueType>& input) : Node({&_input}, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, input.Size())
    {};

    template <typename ValueType>
    void OutputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newOutputPortElements = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<OutputNode<ValueType>>(newOutputPortElements);
        transformer.MapOutputPort(output, newNode->output);
    }

    /// Inherited from ISerializable
    // template <typename ValueType>
    // utilities::ObjectDescription OutputNode<ValueType>::GetDescription() const
    // {
    //     std::cout << "Serializing OutputNode" << std::endl;
    //     auto result = Node::GetDescription();
    //     result.AddField("input", _input);
    //     return result;
    // }

    template <typename ValueType>
    void OutputNode<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("input", _input);
    }
}
