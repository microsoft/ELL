////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DemultiplexerNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    DemultiplexerNode<ValueType, SelectorType>::DemultiplexerNode() : Node({ &_input, &_selector }, { &_output }), _input(this, {}, inputPortName), _selector(this, {}, selectorPortName), _output(this, outputPortName, 0), _defaultValue(0)
    {}

    template <typename ValueType, typename SelectorType>
    DemultiplexerNode<ValueType, SelectorType>::DemultiplexerNode(const model::PortElements<ValueType>& input, const model::PortElements<SelectorType>& selector, size_t outputSize, ValueType defaultValue) : Node({ &_input, &_selector }, { &_output }), _input(this, input, inputPortName), _selector(this, selector, selectorPortName), _output(this, outputPortName, outputSize), _defaultValue(defaultValue)
    {
        if (selector.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }
        if (input.Size() != 1)
        {
            throw std::runtime_error("Error: Input must be 1-D signal");
        }
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::Compute() const
    {
        std::vector<ValueType> outputValue(_output.Size(), _defaultValue);
        int index = (int)_selector[0];
        outputValue[index] = _input[0];
        _output.SetOutput(outputValue);
    }

    template <typename ValueType, typename SelectorType>
    utilities::ObjectDescription  DemultiplexerNode<ValueType, SelectorType>::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<Node,  DemultiplexerNode<ValueType, SelectorType>>("Demultiplexer node");
        description.template AddProperty<decltype(_input)>("input", "Input elements");
        description.template AddProperty<decltype(_selector)>("selector", "Selector value");
        description.template AddProperty<decltype(_output)>("output", "Output port");
        description.template AddProperty<decltype(_defaultValue)>("defaultValue", "Default value");
        return description;
    }

    template <typename ValueType, typename SelectorType>
    utilities::ObjectDescription  DemultiplexerNode<ValueType, SelectorType>::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Node,  DemultiplexerNode<ValueType, SelectorType>>();
        description["input"] = _input;
        description["selector"] = _selector;
        description["output"] = _output;
        description["defaultValue"] = _defaultValue;
        return description;
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description["input"] >> _input;
        description["selector"] >> _selector;
        description["output"] >> _output;
        description["defaultValue"] >> _defaultValue;
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::Serialize(utilities::Serializer& serializer) const
    {
        Node::Serialize(serializer);
        serializer.Serialize("input", _input);
        serializer.Serialize("selector", _selector);
        serializer.Serialize("output", _output);
        serializer.Serialize("defaultValue", _defaultValue);
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::Deserialize(serializer, context);
        serializer.Deserialize("input", _input, context);
        serializer.Deserialize("selector", _selector, context);
        serializer.Deserialize("output", _output, context);
        serializer.Deserialize("defaultValue", _defaultValue, context);
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput= transformer.TransformPortElements(_input.GetPortElements());
        auto newSelector = transformer.TransformPortElements(_selector.GetPortElements());
        auto newNode = transformer.AddNode<DemultiplexerNode<ValueType, SelectorType>>(newInput, newSelector, output.Size(), _defaultValue);
        transformer.MapNodeOutput(output, newNode->output);
    }


    template <typename ValueType>
    model::PortElements<int> CastIfNecessary(const model::PortElements<ValueType>& values, model::ModelTransformer& transformer)
    {
        auto castNode = transformer.AddNode<TypeCastNode<ValueType, int>>(values);
        return castNode->output;
    }

    template <>
    inline model::PortElements<int> CastIfNecessary<int>(const model::PortElements<int>& values, model::ModelTransformer& transformer)
    {
        return values;
    }

    template <typename ValueType, typename SelectorType>
    bool DemultiplexerNode<ValueType, SelectorType>::Refine(model::ModelTransformer& transformer) const
    {        
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newSelector = transformer.TransformPortElements(_selector.GetPortElements());
        auto newSelectorInt = CastIfNecessary(newSelector, transformer);

        auto defaultNode = transformer.AddNode<ConstantNode<ValueType>>(_defaultValue);
        model::PortElements<ValueType> outputElements;
        auto size = _output.Size();
        for(size_t index = 0; index < size; ++index)
        {
            auto indexNode = transformer.AddNode<ConstantNode<int>>(static_cast<int>(index));
            auto isEqualNode = transformer.AddNode<BinaryPredicateNode<int>>(newSelectorInt, indexNode->output, BinaryPredicateNode<int>::PredicateType::equal);
            auto ifNode = transformer.AddNode<nodes::MultiplexerNode<ValueType, bool>>(model::PortElements<ValueType>{defaultNode->output, newInput}, isEqualNode->output);
            outputElements.Append(ifNode->output);
        }
      
        transformer.MapNodeOutput(output, outputElements);
        return true;
    }
}
