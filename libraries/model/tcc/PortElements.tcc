////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PortElements.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // PortElements
    //

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const OutputPort<ValueType>& port) : PortElementsBase(port)
    {
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const OutputPort<ValueType>& port, size_t startIndex) : PortElementsBase(PortRange(port, startIndex))
    {
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues) : PortElementsBase(PortRange(port, startIndex, numValues))
    {
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const std::initializer_list<PortElements<ValueType>>& groups)
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
    PortElements<ValueType>::PortElements(const std::vector<PortElements<ValueType>>& groups)
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
    PortElements<ValueType>::PortElements(const PortElements<ValueType>& elements, size_t index) : PortElements(elements, index, 1) {}

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const PortElements<ValueType>& elements, size_t startIndex, size_t numValues)
    {
        if(startIndex + numValues > elements.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Invalid slice.");
        }

        auto rangeIterator = elements.begin();
        // skip ranges that come before the desired elements
        while(rangeIterator != elements.end() && rangeIterator->Size() <= startIndex)
        {
            startIndex -= rangeIterator->Size();
            ++rangeIterator;
        }

        // now extract portions from ranges until done
        while(rangeIterator != elements.end() && numValues > 0)
        {
            size_t numRangeValues = std::min(rangeIterator->Size()-startIndex, numValues);
            AddRange({*rangeIterator->ReferencedPort(), startIndex, numRangeValues});
            numValues -= numRangeValues;
            ++rangeIterator;
            startIndex = 0; // after the first time through, we'll always take the first part of a range
        }
        ComputeSize();
    }

    template <typename ValueType>
    PortElement<ValueType> PortElements<ValueType>::GetElement(size_t index) const
    {
        auto element = static_cast<PortElement<ValueType>&>(PortElementsBase::GetElement(index));
        return element;
    }

    template <typename ValueType>
    void PortElements<ValueType>::Append(const PortElements<ValueType>& other)
    {
        for(const auto& range: other)
        {
            AddRange(range);
        }
    }


    //
    // Convenience functions
    //

    // MakePortElements
    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port)
    {
        return PortElements<ValueType>(port);
    }

    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port, size_t startIndex)
    {
        return PortElements<ValueType>(port, startIndex);
    }

    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues)
    {
        return PortElements<ValueType>(port, startIndex, numValues);
    }

    // Concat
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs)
    {
        return RefType({ ref1, refs... });
    }
}
