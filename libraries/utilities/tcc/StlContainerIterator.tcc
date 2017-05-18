////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlContainerIterator.tcc (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename IteratorType, typename ValueType>
    StlContainerIteratorBase<IteratorType, ValueType>::StlContainerIteratorBase(IteratorType begin, IteratorType end)
        : _current(begin), _end(end)
    {
    }

    template <typename IteratorType, typename ValueType>
    void StlContainerIteratorBase<IteratorType, ValueType>::Next()
    {
        if (IsValid())
        {
            ++_current;
        }
    }

    template <typename IteratorType>
    StlContainerIterator<IteratorType> MakeStlContainerIterator(IteratorType begin, IteratorType end)
    {
        return StlContainerIterator<IteratorType>(begin, end);
    }

    template <typename ContainerType>
    StlContainerIterator<typename ContainerType::iterator, typename ContainerType::value_type> MakeStlContainerIterator(ContainerType& container)
    {
        return StlContainerIterator<typename ContainerType::iterator, typename ContainerType::value_type>(container.begin(), container.end());
    }

    template <typename IteratorType>
    StlContainerReferenceIterator<IteratorType> MakeStlContainerReferenceIterator(IteratorType begin, IteratorType end)
    {
        return StlContainerReferenceIterator<IteratorType>(begin, end);
    }

    template <typename ContainerType>
    StlContainerReferenceIterator<typename ContainerType::iterator, typename ContainerType::value_type> MakeStlContainerReferenceIterator(ContainerType& container)
    {
        return StlContainerReferenceIterator<typename ContainerType::iterator, typename ContainerType::value_type>(container.begin(), container.end());
    }
}
}
