////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     HammingWindowNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    HammingWindowNode<ValueType>::HammingWindowNode()
        : Node({&_input}, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    HammingWindowNode<ValueType>::HammingWindowNode(const model::PortElements<ValueType>& input)
        : Node({&_input}, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, input.Size())
    {
    }

    template <typename ValueType>
    void HammingWindowNode<ValueType>::Compute() const
    {
        auto size = _input.Size();
        auto window = dsp::HammingWindow<ValueType>(size);
        auto result = std::vector<ValueType>(size);
        for (size_t index = 0; index < size; index++)
        {
            result[index] = _input[index] * window[index];
        }
        _output.SetOutput(result);
    }

    template <typename ValueType>
    void HammingWindowNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<HammingWindowNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool HammingWindowNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto constantNode = transformer.AddNode<ConstantNode<ValueType>>(dsp::HammingWindow<ValueType>(_input.Size()));
        auto multiplyNode = transformer.AddNode<BinaryOperationNode<ValueType>>(newPortElements, constantNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);
        transformer.MapNodeOutput(output, multiplyNode->output);
        return true;
    }

    template <typename ValueType>
    void HammingWindowNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void HammingWindowNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        _output.SetSize(_input.Size());
    }
}
}
