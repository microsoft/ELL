////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPortElementList.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // OutputPortElementList
    //

    template <typename ValueType>
    OutputPortElementList<ValueType>::OutputPortElementList(const OutputPort<ValueType>& port) : OutputPortElementListUntyped(port)
    {
    }

    template <typename ValueType>
    OutputPortElementList<ValueType>::OutputPortElementList(const OutputPort<ValueType>& port, size_t startIndex) : OutputPortElementListUntyped(OutputPortRange(port, startIndex))
    {
    }

    template <typename ValueType>
    OutputPortElementList<ValueType>::OutputPortElementList(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues) : OutputPortElementListUntyped(OutputPortRange(port, startIndex, numValues))
    {
    }

    template <typename ValueType>
    OutputPortElementList<ValueType>::OutputPortElementList(const std::initializer_list<OutputPortElementList<ValueType>>& groups)
    {
        for (const auto& group : groups)
        {
            for (const auto& range : group)
            {
                AddRange(range);
            }
        }
        ComputeSize();
    }

    template <typename ValueType>
    OutputPortElementList<ValueType>::OutputPortElementList(const std::vector<OutputPortElementList<ValueType>>& groups)
    {
        for (const auto& group : groups)
        {
            for (const auto& range : group)
            {
                AddRange(range);
            }
        }
        ComputeSize();
    }

    //
    // Convenience functions
    //

    // MakeOutputPortElementList
    template <typename ValueType>
    OutputPortElementList<ValueType> MakeOutputPortElementList(const OutputPort<ValueType>& port)
    {
        return OutputPortElementList<ValueType>(port);
    }

    template <typename ValueType>
    OutputPortElementList<ValueType> MakeOutputPortElementList(const OutputPort<ValueType>& port, size_t startIndex)
    {
        return OutputPortElementList<ValueType>(port, startIndex);
    }

    template <typename ValueType>
    OutputPortElementList<ValueType> MakeOutputPortElementList(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues)
    {
        return OutputPortElementList<ValueType>(port, startIndex, numValues);
    }

    // Concat
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs)
    {
        return RefType({ ref1, refs... });
    }
}
