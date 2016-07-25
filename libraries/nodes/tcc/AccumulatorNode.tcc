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
    AccumulatorNode<ValueType>::AccumulatorNode(const model::OutputPortElements<ValueType>& input) : Node({&_input}, {&_output}), _input(this, input, inputPortName), _output(this, outputPortName, _input.Size())
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
        auto newInput = transformer.TransformOutputPortElements(_input.GetOutputPortElements());
        auto newNode = transformer.AddNode<AccumulatorNode<ValueType>>(newInput);
        transformer.MapOutputPort(output, newNode->output);
    }
}
