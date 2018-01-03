///////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     L2NormSquaredNode.tcc (nodes)
//  Authors:  Kern Handa
//
///////////////////////////////////////////////////////////////////////////////

#include "UnaryOperationNode.h"
#include "SumNode.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    L2NormSquaredNode<ValueType>::L2NormSquaredNode()
        : Node({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    L2NormSquaredNode<ValueType>::L2NormSquaredNode(const model::PortElements<ValueType>& input)
        : Node({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    void L2NormSquaredNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += (v * v);
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void L2NormSquaredNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<L2NormSquaredNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool L2NormSquaredNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

        auto squareInputNode = transformer.AddNode<UnaryOperationNode<ValueType>>(newPortElements, emitters::UnaryOperationType::square);
        auto sumNode = transformer.AddNode<SumNode<ValueType>>(squareInputNode->output);

        transformer.MapNodeOutput(output, sumNode->output);
        return true;
    }

    template <typename ValueType>
    void L2NormSquaredNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void L2NormSquaredNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
    }
}
}
