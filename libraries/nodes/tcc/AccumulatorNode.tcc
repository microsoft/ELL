////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AccumulatorNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    AccumulatorNode<ValueType>::AccumulatorNode() : Node({&_input}, {&_output}), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    AccumulatorNode<ValueType>::AccumulatorNode(const model::PortElements<ValueType>& input) : Node({&_input}, {&_output}), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size())
    {
        auto dimension = input.Size();
        _accumulator = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::Compute() const
    {
        for(size_t index = 0; index < _input.Size(); ++index)
        {
            _accumulator[index] += _input[index];
        }
        _output.SetOutput(_accumulator);
    };

    template <typename ValueType>
    void AccumulatorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<AccumulatorNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::GetDescription(utilities::ObjectDescription& description) const
    {
        Node::GetDescription(description);
        description.SetType(*this);
        description[inputPortName] << _input;
        description[outputPortName] << _output;
    }

    template <typename ValueType>
    void AccumulatorNode<ValueType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description[inputPortName] >> _input;
        description[outputPortName] >> _output;

        auto dimension = _input.Size();
        _accumulator = std::vector<ValueType>(dimension);
    }
}
