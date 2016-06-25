////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPortRef.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // OutputPortRef
    //

    template <typename ValueType>
    OutputPortRef<ValueType>::OutputPortRef(const OutputPort<ValueType>& port) : OutputPortRefUntyped(port)
    {
    }

    template <typename ValueType>
    OutputPortRef<ValueType>::OutputPortRef(const OutputPort<ValueType>& port, size_t startIndex) : OutputPortRefUntyped(OutputPortRange(port, startIndex))
    {
    }

    template <typename ValueType>
    OutputPortRef<ValueType>::OutputPortRef(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues) : OutputPortRefUntyped(OutputPortRange(port, startIndex, numValues))
    {
    }

    template <typename ValueType>
    OutputPortRef<ValueType>::OutputPortRef(const std::initializer_list<OutputPortRef<ValueType>>& groups)
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
    OutputPortRef<ValueType>::OutputPortRef(const std::vector<OutputPortRef<ValueType>>& groups)
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

    // MakeRef
    template <typename ValueType>
    OutputPortRef<ValueType> MakeRef(const OutputPort<ValueType>& port)
    {
        return OutputPortRef<ValueType>(port);
    }

    template <typename ValueType>
    OutputPortRef<ValueType> MakeRef(const OutputPort<ValueType>& port, size_t startIndex)
    {
        return OutputPortRef<ValueType>(port, startIndex);
    }

    template <typename ValueType>
    OutputPortRef<ValueType> MakeRef(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues)
    {
        return OutputPortRef<ValueType>(port, startIndex, numValues);
    }

    // Concat
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs)
    {
        return RefType({ ref1, refs... });
    }
}
