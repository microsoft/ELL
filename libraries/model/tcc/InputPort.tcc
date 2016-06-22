////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

/// <summary> model namespace </summary>
namespace model
{

    //
    // TypedInputGroup
    //

    template <typename ValueType>
    TypedInputGroup<ValueType>::TypedInputGroup(const OutputPort<ValueType>& port) : InputGroup(port) {}

    template <typename ValueType>
    TypedInputGroup<ValueType>::TypedInputGroup(const OutputPort<ValueType>& port, size_t startIndex) : InputGroup(InputRange(port, startIndex)) {}

    template <typename ValueType>
    TypedInputGroup<ValueType>::TypedInputGroup(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues) : InputGroup(InputRange(port, startIndex, numValues)) {}

    template <typename ValueType>
    TypedInputGroup<ValueType>::TypedInputGroup(const std::initializer_list<TypedInputGroup<ValueType>>& groups)
    {
        for (const auto& group : groups)
        {
            for (const auto& range : group._ranges)
            {
                _ranges.push_back(range);
            }
        }
        ComputeSize();
    }

    template <typename ValueType>
    TypedInputGroup<ValueType>::TypedInputGroup(const std::vector<TypedInputGroup<ValueType>>& groups)
    {
        for (const auto& group : groups)
        {
            for (const auto& range : group._ranges)
            {
                _ranges.push_back(range);
            }
        }
        ComputeSize();
    }

    //
    // InputPort
    //
    template <typename ValueType>
    InputPort::InputPort(const class Node* owningNode, size_t portIndex, const TypedInputGroup<ValueType>& input) : Port(owningNode, portIndex, Port::GetTypeCode<ValueType>(), input.Size()), _inputRanges(input)
    {
    }

    template <typename ValueType>
    std::vector<ValueType> InputPort::GetValue() const
    {
        std::vector<ValueType> result;
        result.reserve(Size());
        for (const auto& range : _inputRanges)
        {
            auto temp = range._referencedPort->Node()->GetOutputValue<ValueType>(range._referencedPort->Index());
            if (range._isFixedSize)
            {
                result.insert(result.end(), temp.begin() + range._startIndex, temp.begin() + range._startIndex + range._numValues);
            }
            else
            {
                result.insert(result.end(), temp.begin(), temp.end());
            }
        }

        assert(Size() == result.size());
        return result;
    }
}
