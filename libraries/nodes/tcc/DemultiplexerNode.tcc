////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DemultiplexerNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    template <typename ValueType, typename SelectorType>
    DemultiplexerNode<ValueType, SelectorType>::DemultiplexerNode()
        : Node({ &_input, &_selector }, { &_output }), _input(this, {}, defaultInputPortName), _selector(this, {}, selectorPortName), _output(this, defaultOutputPortName, 0), _defaultValue(0)
    {
    }

    template <typename ValueType, typename SelectorType>
    DemultiplexerNode<ValueType, SelectorType>::DemultiplexerNode(const model::PortElements<ValueType>& input, const model::PortElements<SelectorType>& selector, size_t outputSize, ValueType defaultValue)
        : Node({ &_input, &_selector }, { &_output }), _input(this, input, defaultInputPortName), _selector(this, selector, selectorPortName), _output(this, defaultOutputPortName, outputSize), _defaultValue(defaultValue)
    {
        if (selector.Size() != 1)
        {
            throw ell::utilities::Exception("Error: Condition must be 1-D signal");
        }
        if (input.Size() != 1)
        {
            throw ell::utilities::Exception("Error: Input must be 1-D signal");
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
    void DemultiplexerNode<ValueType, SelectorType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[selectorPortName] << _selector;
        archiver["size"] << _output.Size();
        archiver["defaultValue"] << _defaultValue;
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[selectorPortName] >> _selector;
        size_t size;
        archiver["size"] >> size;
        _output.SetSize(size);
        archiver["defaultValue"] >> _defaultValue;
    }

    template <typename ValueType, typename SelectorType>
    void DemultiplexerNode<ValueType, SelectorType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
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
        for (size_t index = 0; index < size; ++index)
        {
            auto indexNode = transformer.AddNode<ConstantNode<int>>(static_cast<int>(index));
            auto isEqualNode = transformer.AddNode<BinaryPredicateNode<int>>(newSelectorInt, indexNode->output, emitters::BinaryPredicateType::equal);
            auto ifNode = transformer.AddNode<nodes::MultiplexerNode<ValueType, bool>>(model::PortElements<ValueType>{ defaultNode->output, newInput }, isEqualNode->output);
            outputElements.Append(ifNode->output);
        }

        transformer.MapNodeOutput(output, outputElements);
        return true;
    }
}
}
