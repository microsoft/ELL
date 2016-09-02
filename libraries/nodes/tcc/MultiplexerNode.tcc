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
    void  MultiplexerNode<ValueType, SelectorType>::AddProperties(utilities::Archiver& archiver) const
    {
        Node::AddProperties(archiver);
        archiver["elements"] << _elements;
        archiver["selector"] << _selector;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType, typename SelectorType>
    void  MultiplexerNode<ValueType, SelectorType>::SetObjectState(const utilities::Archiver& archiver, utilities::SerializationContext& context)
    {
        Node::SetObjectState(archiver, context);
        archiver["elements"] >> _elements;
        archiver["selector"] >> _selector;
        archiver[outputPortName] >> _output;
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
