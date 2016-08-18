////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryMultiplexorNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace nodes
{
    template <typename ValueType>
    BinaryMultiplexorNode<ValueType>::BinaryMultiplexorNode(const model::PortElements<ValueType>& input, const model::PortElements<bool>& selector, ValueType defaultValue) : Node({ &_input, &_selector }, { &_output1, &_output2 }), _input(this, input, inputPortName), _selector(this, selector, selectorPortName), _output1(this, output1PortName, _input.Size()), _output2(this, output2PortName, _input.Size())
    {
        _defaultValue = std::vector<ValueType>(_input.Size(), defaultValue);
        if (selector.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }
    };

    template <typename ValueType>
    void BinaryMultiplexorNode<ValueType>::Compute() const
    {
        bool condition = _selector[0];
        if(condition == false)
        {
            _output1.SetOutput(_input.GetValue());
            _output2.SetOutput(_defaultValue);
        }
        else
        {
            _output1.SetOutput(_defaultValue);
            _output2.SetOutput(_input.GetValue());
        }
    }

    template <typename ValueType>
    void BinaryMultiplexorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newSelector = transformer.TransformPortElements(_selector.GetPortElements());
        auto newNode = transformer.AddNode<BinaryMultiplexorNode<ValueType>>(newElements, newSelector, _defaultValue[0]);
        transformer.MapNodeOutput(output1, newNode->output1);
        transformer.MapNodeOutput(output2, newNode->output2);
    }
}
