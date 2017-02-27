////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     L2NormNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    L2NormNode<ValueType>::L2NormNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    L2NormNode<ValueType>::L2NormNode(const model::PortElements<ValueType>& input)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    void L2NormNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += (v * v);
        }
        _output.SetOutput({ std::sqrt(result) });
    };

    template <typename ValueType>
    void L2NormNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<L2NormNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void L2NormNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
    }

    template <typename ValueType>
    void L2NormNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
    }
}
}
