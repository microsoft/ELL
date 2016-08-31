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
    utilities::ObjectDescription  MultiplexerNode<ValueType, SelectorType>::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<Node,  MultiplexerNode<ValueType, SelectorType>>("Multiplexer node");
        description.template AddProperty<decltype(_elements)>("elements", "Input elements");
        description.template AddProperty<decltype(_selector)>("selector", "Selector value");
        description.template AddProperty<decltype(_output)>(outputPortName, "Output port");
        return description;
    }

    template <typename ValueType, typename SelectorType>
    utilities::ObjectDescription  MultiplexerNode<ValueType, SelectorType>::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Node,  MultiplexerNode<ValueType, SelectorType>>();
        description["elements"] = _elements;
        description["selector"] = _selector;
        description[outputPortName] = _output;
        return description;
    }

    template <typename ValueType, typename SelectorType>
    void  MultiplexerNode<ValueType, SelectorType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description["elements"] >> _elements;
        description["selector"] >> _selector;
        description[outputPortName] >> _output;
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
