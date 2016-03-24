////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StlIterator.tcc (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
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
    ValueType StlIterator<IteratorType, ValueType>::Get() const
    {
        return *_current;
    }
 
    template <typename IteratorType>
    auto MakeStlIterator(IteratorType begin, IteratorType end) -> StlIterator<IteratorType, decltype(*begin)>
    {
        return StlIterator<IteratorType>(begin, end);
    }

    template <typename ContainerType>
    auto MakeStlIterator(ContainerType& container) -> StlIterator<typename ContainerType::iterator, typename ContainerType::value_type>
    {
        return StlIterator<typename ContainerType::iterator>(container.begin(), container.end());
    }

}
