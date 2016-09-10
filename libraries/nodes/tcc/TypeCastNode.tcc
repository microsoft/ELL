////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TypeCastNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
/// <summary> model namespace </summary>
namespace nodes
{
    template <typename InputValueType, typename OutputValueType>
    TypeCastNode<InputValueType, OutputValueType>::TypeCastNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0){};

    template <typename InputValueType, typename OutputValueType>
    TypeCastNode<InputValueType, OutputValueType>::TypeCastNode(const model::PortElements<InputValueType>& input)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, input.Size()){};

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::Compute() const
    {
        auto size = _output.Size();
        std::vector<OutputValueType> outputValues(size);
        for (size_t index = 0; index < size; ++index)
        {
            outputValues[index] = static_cast<OutputValueType>(_input[index]);
        }
        _output.SetOutput(outputValues);
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<TypeCastNode<InputValueType, OutputValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
    }

    template <typename InputValueType, typename OutputValueType>
    void TypeCastNode<InputValueType, OutputValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
    }
}
}
