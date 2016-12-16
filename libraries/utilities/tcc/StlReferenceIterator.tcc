////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StlReferenceIterator.tcc (utilities)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename IteratorType, typename ValueType>
    StlReferenceIterator<IteratorType, ValueType>::StlReferenceIterator(IteratorType begin, IteratorType end)
        : _current(begin), _end(end)
    {
    }

    template <typename IteratorType, typename ValueType>
    void StlReferenceIterator<IteratorType, ValueType>::Next()
    {
        if (IsValid())
        {
            ++_current;
        }
    }

    template <typename IteratorType>
    StlReferenceIterator<IteratorType> MakeStlReferenceIterator(IteratorType begin, IteratorType end)
    {
        return StlReferenceIterator<IteratorType>(begin, end);
    }

    template <typename ContainerType>
    StlReferenceIterator<typename ContainerType::iterator, typename ContainerType::value_type> MakeStlReferenceIterator(ContainerType& container)
    {
        return StlReferenceIterator<typename ContainerType::iterator, typename ContainerType::value_type>(container.begin(), container.end());
    }
}
}
