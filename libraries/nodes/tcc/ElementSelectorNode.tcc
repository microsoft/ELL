////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ElementSelectorNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace nodes
{
    template <typename ValueType>
    ElementSelectorNode<ValueType>::ElementSelectorNode(const model::OutputPortElements<ValueType>& input, const model::OutputPortElements<int>& selector) : Node({ &_elements, &_selector }, { &_output }), _elements(this, input, elementsPortName), _selector(this, selector, selectorPortName), _output(this, outputPortName, 1)
    {
        if (selector.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }
    };

    template <typename ValueType>
    void ElementSelectorNode<ValueType>::Compute() const
    {
        int index = _selector[0];
        _output.SetOutput({ _elements[index] });
    }

    template <typename ValueType>
    void ElementSelectorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newElements = transformer.TransformOutputPortElements(_elements.GetOutputPortElements());
        auto newSelector = transformer.TransformOutputPortElements(_selector.GetOutputPortElements());
        auto newNode = transformer.AddNode<ElementSelectorNode<ValueType>>(newElements, newSelector);
        transformer.MapOutputPort(output, newNode->output);
    }
}
