////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiplexorNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    MultiplexorNode<ValueType, SelectorType>::MultiplexorNode(const model::PortElements<ValueType>& input, const model::PortElements<SelectorType>& selector, size_t outputSize, ValueType defaultValue) : Node({ &_input, &_selector }, { &_output }), _input(this, input, inputPortName), _selector(this, selector, selectorPortName), _output(this, outputPortName, outputSize), _defaultValue(defaultValue)
    {
        if (selector.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }
        if (input.Size() != 1)
        {
            throw std::runtime_error("Error: Input must be 1-D signal");
        }
    };

    template <typename ValueType, typename SelectorType>
    void MultiplexorNode<ValueType, SelectorType>::Compute() const
    {
        std::vector<ValueType> outputValue(_output.Size(), _defaultValue);
        int index = (int)_selector[0];
        outputValue[index] = _input[0];
        _output.SetOutput(outputValue);
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexorNode<ValueType, SelectorType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput= transformer.TransformPortElements(_input.GetPortElements());
        auto newSelector = transformer.TransformPortElements(_selector.GetPortElements());
        auto newNode = transformer.AddNode<MultiplexorNode<ValueType, SelectorType>>(newInput, newSelector, output.Size(), _defaultValue);
        transformer.MapNodeOutput(output, newNode->output);
    }
}
