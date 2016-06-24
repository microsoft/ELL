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
    InputPortBase::InputPortBase(const class Node* owningNode, size_t portIndex, const OutputRef<ValueType>& input) : Port(owningNode, portIndex, Port::GetTypeCode<ValueType>(), input.Size()), _inputRanges(input)
    {
    }

    template <typename ValueType>
    std::vector<ValueType> InputPortBase::GetTypedValue() const
    {
        std::vector<ValueType> result;
        result.reserve(Size());
        for (const auto& range : _inputRanges)
        {
            auto temp = range.ReferencedPort()->Node()->GetOutputValue<ValueType>(range.ReferencedPort()->Index());
            result.insert(result.end(), temp.begin() + range.StartIndex(), temp.begin() + range.StartIndex() + range.Size());
        }

        assert(Size() == result.size());
        return result;
    }

    template <typename ValueType>
    ValueType InputPortBase::GetTypedValue(size_t index) const
    {
        // TODO: be less inefficient about this!
        return GetTypedValue<ValueType>()[index];
    }

    //
    // InputPort
    // 
    template <typename ValueType>
    InputPort<ValueType>::InputPort(const class Node* owningNode, size_t portIndex, const OutputRef<ValueType>& input) : InputPortBase(owningNode, portIndex, input) {}

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
