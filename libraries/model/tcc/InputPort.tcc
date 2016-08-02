////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // InputPortBase
    //
    template <typename ValueType>
    InputPortBase::InputPortBase(const class Node* owningNode, const OutputPortElements<ValueType>& inputRef, const OutputPortElements<ValueType>& inputValues, std::string name) : Port(owningNode, name, Port::GetPortType<ValueType>(), inputValues.Size()), _inputRanges(inputRef)
    {
        assert(owningNode != nullptr);
        for (const auto& range : inputValues)
        {
            auto port = range.ReferencedPort();
            auto node = port->GetNode();
            auto start = range.GetStartIndex();
            auto numElements = range.Size();
            for (size_t index = 0; index < numElements; ++index)
            {
                _individualElements.emplace_back(*port, index + start);
            }
            _parentNodes.push_back(node);
        }
    }

    template <typename ValueType>
    std::vector<ValueType> InputPortBase::GetTypedValue() const
    {
        std::vector<ValueType> result;
        result.reserve(Size());
        for (const auto& element : _individualElements)
        {
            auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
            auto temp = typedOutput->GetOutput(element.GetIndex());
            result.push_back(temp);
        }

        if(Size() != result.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return result;
    }

    template <typename ValueType>
    ValueType InputPortBase::GetTypedValue(size_t index) const
    {
        const auto& element = _individualElements[index];
        auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
        return typedOutput->GetOutput(element.GetIndex());
    }

    //
    // InputPort
    //
    template <typename ValueType>
    InputPort<ValueType>::InputPort(const class Node* owningNode, const OutputPortElements<ValueType>& input, std::string name) : InputPortBase(owningNode, _input, input, name), _input(input)
    {
    }

    template <typename ValueType>
    ValueType InputPort<ValueType>::GetValue(size_t index) const
    {
        return GetTypedValue<ValueType>(index);
    }

    template <typename ValueType>
    ValueType InputPort<ValueType>::operator[](size_t index) const
    {
        return GetTypedValue<ValueType>(index);
    }

    template <typename ValueType>
    OutputPortElements<ValueType> InputPort<ValueType>::GetOutputPortElements() const
    {
        return _input;
    }

    template <typename ValueType>
    void InputPort<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        InputPortBase::Serialize(serializer); // call this SerializeContents
        serializer.Serialize("inputElements", _input);
    }

    template <typename ValueType>
    void InputPort<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
         InputPortBase::Deserialize(serializer, context);

        serializer.Deserialize("inputElements", _input, context);
    }
}
