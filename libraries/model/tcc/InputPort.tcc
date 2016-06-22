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
    InputPort::InputPort(const class Node* owningNode, size_t portIndex, const OutputRef<ValueType>& input) : Port(owningNode, portIndex, Port::GetTypeCode<ValueType>(), input.Size()), _inputRanges(input)
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
