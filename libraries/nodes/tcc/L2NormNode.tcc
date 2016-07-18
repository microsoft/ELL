////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     L2NormNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    L2NormNode<ValueType>::L2NormNode(const model::OutputPortElementList<ValueType>& input) : Node({&_input}, {&_output}), _input(this, input), _output(this, 1)
    {
    }

    template <typename ValueType>
    void L2NormNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for(size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += (v*v);
        }
        _output.SetOutput({std::sqrt(result)});
    };

    template <typename ValueType>
    void L2NormNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<L2NormNode<ValueType>>(newInput);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void L2NormNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        // TODO: elementwise x^2, sum, div by D
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<L2NormNode<ValueType>>(newInput);
        transformer.MapOutputPort(output, newNode->output);
    }
}
