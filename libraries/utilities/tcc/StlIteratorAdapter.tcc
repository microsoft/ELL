////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StlIteratorAdapter.tcc (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    template <typename IteratorType, typename ValueType>
    StlIteratorAdapter<IteratorType, ValueType>::StlIteratorAdapter(IteratorType begin, IteratorType end) : _current(begin), _end(end)
    {}

    template <typename IteratorType, typename ValueType>
    bool StlIteratorAdapter<IteratorType, ValueType>::IsValid() const
    {
        return (_current != _end);
    }

    template <typename IteratorType, typename ValueType>
    void StlIteratorAdapter<IteratorType, ValueType>::Next()
    {
        if (IsValid())
        {
            ++_current;
        }
    }

    template <typename IteratorType, typename ValueType>
    ValueType StlIteratorAdapter<IteratorType, ValueType>::Get() const
    {
        return *_current;
    }

    template <typename IteratorType>
    auto MakeStlIteratorAdapter(IteratorType begin, IteratorType end) -> StlIteratorAdapter<IteratorType, decltype(*begin)>
    {
        return StlIteratorAdapter<IteratorType>(begin, end);
    }

    template <typename ContainerType>
    auto MakeStlIteratorAdapter(ContainerType& container) -> StlIteratorAdapter<typename ContainerType::iterator, typename ContainerType::value_type>
    {
        return StlIteratorAdapter<typename ContainerType::iterator>(container.begin(), container.end());
    }

}
