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
    OutputNode<ValueType>::OutputNode(const model::OutputPortElementList<ValueType>& input) : Node({&_input}, { &_output }), _input(this, input, "input"), _output(this, "output", input.Size()){};

    template <typename ValueType>
    void OutputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<OutputNode<ValueType>>(newInput);
        transformer.MapOutputPort(output, newNode->output);
    }

    /// Inherited from ISerializable
    template <typename ValueType>
    utilities::ObjectDescription OutputNode<ValueType>::GetDescription() const
    {
        std::cout << "Serializing OutputNode" << std::endl;
        auto result = Node::GetDescription();
        result.AddField("input", _input);
        return result;
    }
}
