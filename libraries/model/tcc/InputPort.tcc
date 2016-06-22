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

    template <typename ValueType>
    InputPort::InputPort(const class Node* owningNode, size_t portIndex, const TypedRange<ValueType>& input) : Port(owningNode, portIndex, Port::GetTypeCode<ValueType>(), input.Size())
    {
        _inputRanges.emplace_back(InputRange(input));
    }

    template <typename ValueType>
    InputPort::InputPort(const class Node* owningNode, size_t portIndex, const TypedInputGroup<ValueType>& input) : Port(owningNode, portIndex, Port::GetTypeCode<ValueType>(), input.Size())
    {
        _inputRanges.insert(_inputRanges.begin(), input.begin(), input.end());
    }

    template <typename ValueType>
    std::vector<ValueType> InputPort::GetValue() const
    {
        std::vector<ValueType> result;
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

        return result;
    }
}
