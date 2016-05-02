////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     StlIterator.tcc (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template <typename IteratorType, typename ValueType>
    StlIterator<IteratorType, ValueType>::StlIterator(IteratorType begin, IteratorType end) : _current(begin), _end(end)
    {}

    template <typename IteratorType, typename ValueType>
    bool StlIterator<IteratorType, ValueType>::IsValid() const
    {
        return (_current != _end);
    }

    template <typename IteratorType, typename ValueType>
    void StlIterator<IteratorType, ValueType>::Next()
    {
        if (IsValid())
        {
            ++_current;
        }
    }

    template <typename IteratorType, typename ValueType>
    bool StlIterator<IteratorType, ValueType>::HasSize() const
    {
        return true;
    }

    template <typename IteratorType, typename ValueType>
    uint64_t StlIterator<IteratorType, ValueType>::NumIteratesLeft() const
    {
        return _end - _current;
    }

    template <typename IteratorType, typename ValueType>
    ValueType StlIterator<IteratorType, ValueType>::Get() const
    {
        return *_current;
    }

    template <typename IteratorType>
    StlIterator<IteratorType> MakeStlIterator(IteratorType begin, IteratorType end)
    {
        return StlIterator<IteratorType>(begin, end);
    }

    template <typename ContainerType>
    StlIterator<typename ContainerType::iterator, typename ContainerType::value_type> MakeStlIterator(ContainerType& container)
    {
        return StlIterator<typename ContainerType::iterator, typename ContainerType::value_type>(container.begin(), container.end());
    }

}
