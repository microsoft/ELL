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
    OutputNode<ValueType>::OutputNode() : Node({&_input}, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {};

    template <typename ValueType>
    OutputNode<ValueType>::OutputNode(const model::PortElements<ValueType>& input) : Node({&_input}, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, input.Size())
    {};

    template <typename ValueType>
    void OutputNode<ValueType>::Compute() const
    {
        _output.SetOutput(_input.GetValue());
    }

    template <typename ValueType>
    void OutputNode<ValueType>::Copy(ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<OutputNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void OutputNode<ValueType>::AddProperties(utilities::Archiver& archiver) const
    {
        Node::AddProperties(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType>
    void OutputNode<ValueType>::SetObjectState(const utilities::Archiver& archiver, utilities::SerializationContext& context)
    {
        Node::SetObjectState(archiver, context);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
    }
}
