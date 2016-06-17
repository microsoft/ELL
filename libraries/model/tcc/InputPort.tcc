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
    // InputRange
    //
    template <typename ValueType>
    InputRange::InputRange(const OutputPort<ValueType>& port) : referencedPort(static_cast<const Port*>(&port)), startIndex(0), numValues(port.Size()), isFixedSize(false)
    {
    }

    template <typename ValueType>
    InputRange::InputRange(const OutputPort<ValueType>& port, size_t index) : referencedPort(static_cast<const Port*>(&port)), startIndex(index), numValues(1), isFixedSize(true)
    {
    }

    template <typename ValueType>
    InputRange::InputRange(const OutputPort<ValueType>& port, size_t index, size_t numValues) : referencedPort(static_cast<const Port*>(&port)), startIndex(index), numValues(numValues), isFixedSize(true)
    {
    }

    //
    // InputPort
    //

    template <typename ValueType>
    InputPort::InputPort(const class Node* owningNode, size_t portIndex, const OutputPort<ValueType>* output) : Port(owningNode, portIndex, Port::GetTypeCode<ValueType>(), output->Size())
    {
        _inputRanges.emplace_back(InputRange(*output));
 
         if (Port::GetTypeCode<ValueType>() != output->Type())
        {
            throw std::runtime_error("InputPort type doesn't match output");
        }
    }

    template <typename ValueType>
    std::vector<ValueType> InputPort::GetValue() const
    {
        size_t size = 0;
        std::vector<ValueType> result;
        for (const auto& range : _inputRanges)
        {
            auto temp = range.referencedPort->Node()->GetOutputValue<ValueType>(range.referencedPort->Index());
            if (range.isFixedSize)
            {
                size += range.referencedPort->Size();
                result.insert(result.end(), temp.begin()+range.startIndex, temp.end()+range.startIndex+range.numValues);

            }
            else
            {
                size += range.referencedPort->Size();
                result.insert(result.end(), temp.begin(), temp.end());
            }
        }

        return result;
    }
}
