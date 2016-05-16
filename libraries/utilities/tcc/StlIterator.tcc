////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
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
    void StlIterator<IteratorType, ValueType>::Next()
    {
        if (IsValid())
        {
            ++_current;
        }
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
