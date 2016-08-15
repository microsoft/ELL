////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ElementSelectorNode.tcc (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    ElementSelectorNode<ValueType, SelectorType>::ElementSelectorNode(const model::PortElements<ValueType>& input, const model::PortElements<SelectorType>& selector) : Node({ &_elements, &_selector }, { &_output }), _elements(this, input, elementsPortName), _selector(this, selector, selectorPortName), _output(this, outputPortName, 1)
    {
        if (selector.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }
    };

    template <typename ValueType, typename SelectorType>
    void ElementSelectorNode<ValueType, SelectorType>::Compute() const
    {
        int index = static_cast<int>(_selector[0]);
        _output.SetOutput({ _elements[index] });
    }

    template <typename ValueType, typename SelectorType>
    void ElementSelectorNode<ValueType, SelectorType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newElements = transformer.TransformPortElements(_elements.GetPortElements());
        auto newSelector = transformer.TransformPortElements(_selector.GetPortElements());
        auto newNode = transformer.AddNode<ElementSelectorNode<ValueType, SelectorType>>(newElements, newSelector);
        transformer.MapOutputPort(output, newNode->output);
    }
}
