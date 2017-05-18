////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortElements.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    //
    // PortElements
    //

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const OutputPort<ValueType>& port)
        : PortElementsBase(port)
    {
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const OutputPort<ValueType>& port, size_t startIndex)
        : PortElementsBase(PortRange(port, startIndex))
    {
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues)
        : PortElementsBase(PortRange(port, startIndex, numValues))
    {
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const PortElement<ValueType>& element)
    {
        AddRange(PortRange(*element.ReferencedPort(), element.GetIndex(), 1));
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const std::vector<PortElement<ValueType>>& elements)
    {
        for (const auto& element : elements)
        {
            AddRange({ element.ReferencedPort(), element.GetIndex() });
        }
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const std::initializer_list<PortElements<ValueType>>& groups)
    {
        for (const auto& group : groups)
        {
            for (const auto& range : group.GetRanges())
            {
                AddRange(range);
            }
        }
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const std::vector<PortElements<ValueType>>& groups)
    {
        for (const auto& group : groups)
        {
            for (const auto& range : group.GetRanges())
            {
                AddRange(range);
            }
        }
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const PortElements<ValueType>& elements, size_t index)
        : PortElements(elements, index, 1)
    {
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const PortElements<ValueType>& elements, size_t startIndex, size_t numValues)
    {
        if (startIndex + numValues > elements.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Invalid slice.");
        }

        auto rangeIterator = elements.GetRanges().begin();
        auto endIterator = elements.GetRanges().end();
        // skip ranges that come before the desired elements
        while (rangeIterator != endIterator && rangeIterator->Size() <= startIndex)
        {
            startIndex -= rangeIterator->Size();
            ++rangeIterator;
        }

        // now extract portions from ranges until done
        while (rangeIterator != endIterator && numValues > 0)
        {
            size_t numRangeValues = std::min(rangeIterator->Size() - startIndex, numValues);
            AddRange({ *rangeIterator->ReferencedPort(), startIndex, numRangeValues });
            numValues -= numRangeValues;
            ++rangeIterator;
            startIndex = 0; // after the first time through, we'll always take the first part of a range
        }
        ComputeSize();
    }

    template <typename ValueType>
    PortElements<ValueType>::PortElements(const PortElementsBase& other)
    {
        for (const auto& range : other.GetRanges())
        {
            if (range.GetPortType() != Port::GetPortType<ValueType>())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }
            AddRange(range);
        }
    }

    template <typename ValueType>
    PortElement<ValueType> PortElements<ValueType>::GetElement(size_t index) const
    {
        auto baseElement = PortElementsBase::GetElement(index);
        auto element = static_cast<PortElement<ValueType>&>(baseElement);
        return element;
    }

    template <typename ValueType>
    void PortElements<ValueType>::Append(const PortElements<ValueType>& other)
    {
        PortElementsBase::Append(other);
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

    //
    // Proxy classes
    //
    template <typename ValueType>
    PortElementsProxy PortElementsToProxy(const PortElements<ValueType>& elements)
    {
        PortElementsProxy proxy(elements.GetPortType());
        for (auto r : elements.GetRanges())
        {
            proxy.Append(r);
        }
        return proxy;
    }

    template <typename ValueType>
    PortElements<ValueType> ProxyToPortElements(const Model& model, const PortElementsProxy& proxy)
    {
        return PortElements<ValueType>(ProxyToPortElements(model, proxy));
    }

}
}
