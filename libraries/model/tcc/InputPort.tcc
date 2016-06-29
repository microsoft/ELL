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
    // InputPort
    //
    template <typename ValueType>
    InputPortBase::InputPortBase(const class Node* owningNode, const OutputPortElementList<ValueType>& input) : Port(owningNode, Port::GetTypeCode<ValueType>(), input.Size()), _inputRanges(input)
    {
        assert(owningNode != nullptr);
        for (const auto& range : input)
        {
            auto port = range.ReferencedPort();
            auto node = port->GetNode();
            auto start = range.GetStartIndex();
            auto numElements = range.Size();
            for (size_t index = 0; index < numElements; ++index)
            {
                _inputElements.emplace_back(*port, index + start);
            }
            _inputNodes.push_back(node);
        }
    }

    template <typename ValueType>
    std::vector<ValueType> InputPortBase::GetTypedValue() const
    {
        std::vector<ValueType> result;
        result.reserve(Size());
        for (const auto& element : _inputElements)
        {
            auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
            auto temp = typedOutput->GetOutput(element.GetIndex());
            result.push_back(temp);
        }

        assert(Size() == result.size());
        return result;
    }

    template <typename ValueType>
    ValueType InputPortBase::GetTypedValue(size_t index) const
    {
        const auto& element = _inputElements[index];
        auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
        return typedOutput->GetOutput(element.GetIndex());
    }

    //
    // InputPort
    //
    template <typename ValueType>
    InputPort<ValueType>::InputPort(const class Node* owningNode, const OutputPortElementList<ValueType>& input) : InputPortBase(owningNode, input)
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
}
