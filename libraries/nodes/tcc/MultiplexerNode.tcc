////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiplexerNode.tcc (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    MultiplexerNode<ValueType, SelectorType>::MultiplexerNode() : Node({ &_elements, &_selector }, { &_output }), _elements(this, {}, elementsPortName), _selector(this, {}, selectorPortName), _output(this, outputPortName, 1)
    {}
    
    template <typename ValueType, typename SelectorType>
    MultiplexerNode<ValueType, SelectorType>::MultiplexerNode(const model::PortElements<ValueType>& input, const model::PortElements<SelectorType>& selector) : Node({ &_elements, &_selector }, { &_output }), _elements(this, input, elementsPortName), _selector(this, selector, selectorPortName), _output(this, outputPortName, 1)
    {
        if (selector.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }
    };

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::Compute() const
    {
        int index = static_cast<int>(_selector[0]);
        _output.SetOutput({ _elements[index] });
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("elements", _elements);
        serializer.Serialize("selector", _selector);
        serializer.Serialize("output", _output);
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("elements", _elements, context);
        serializer.Deserialize("selector", _selector, context);
        serializer.Deserialize("output", _output, context);
    }

    template <typename ValueType, typename SelectorType>
    void MultiplexerNode<ValueType, SelectorType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newElements = transformer.TransformPortElements(_elements.GetPortElements());
        auto newSelector = transformer.TransformPortElements(_selector.GetPortElements());
        auto newNode = transformer.AddNode<MultiplexerNode<ValueType, SelectorType>>(newElements, newSelector);
        transformer.MapNodeOutput(output, newNode->output);
    }
}
