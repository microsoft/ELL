////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPortElements.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // OutputPortElements
    //

    template <typename ValueType>
    OutputPortElements<ValueType>::OutputPortElements(const OutputPort<ValueType>& port) : OutputPortElementsUntyped(port)
    {
    }

    template <typename ValueType>
    OutputPortElements<ValueType>::OutputPortElements(const OutputPort<ValueType>& port, size_t startIndex) : OutputPortElementsUntyped(OutputPortRange(port, startIndex))
    {
    }

    template <typename ValueType>
    OutputPortElements<ValueType>::OutputPortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues) : OutputPortElementsUntyped(OutputPortRange(port, startIndex, numValues))
    {
    }

    template <typename ValueType>
    OutputPortElements<ValueType>::OutputPortElements(const std::initializer_list<OutputPortElements<ValueType>>& groups)
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
    OutputPortElements<ValueType>::OutputPortElements(const std::vector<OutputPortElements<ValueType>>& groups)
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
    OutputPortElements<ValueType> OutputPortElements<ValueType>::GetSlice(size_t startIndex, size_t numValues) const
    {
        if(startIndex + numValues > Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Invalid slice.");
        }

        size_t currentBegin = 0;
        auto rangeIterator = begin();
        // skip ranges that come before the desired elements
        while(rangeIterator != end() && currentBegin+rangeIterator->Size() <= startIndex)
        {
            startIndex -= rangeIterator->Size();
            ++rangeIterator;
        }

        // now extract stuff from ranges until done
        OutputPortElements<ValueType> result;
        while(rangeIterator != end() && numValues > 0)
        {
            auto currentRange = *rangeIterator;
            size_t numRangeValues = std::min(currentRange.Size()-startIndex, numValues);
            result.AddRange({currentRange->OutputPortBase(), startIndex, numRangeValues});
            numValues -= numRangeValues;
        }
        return result;
    }


    //
    // Convenience functions
    //

    // MakeOutputPortElements
    template <typename ValueType>
    OutputPortElements<ValueType> MakeOutputPortElements(const OutputPort<ValueType>& port)
    {
        return OutputPortElements<ValueType>(port);
    }

    template <typename ValueType>
    OutputPortElements<ValueType> MakeOutputPortElements(const OutputPort<ValueType>& port, size_t startIndex)
    {
        return OutputPortElements<ValueType>(port, startIndex);
    }

    template <typename ValueType>
    OutputPortElements<ValueType> MakeOutputPortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues)
    {
        return OutputPortElements<ValueType>(port, startIndex, numValues);
    }

    // Concat
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs)
    {
        return RefType({ ref1, refs... });
    }
}
