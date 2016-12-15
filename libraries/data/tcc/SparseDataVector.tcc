////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SparseDataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Exception.h"

namespace ell
{
namespace data
{
    template <typename ElementType, typename IntegerListType>
    void SparseDataVector<ElementType, IntegerListType>::Iterator::Next()
    {
        _index_iterator.Next();
        ++_value_iterator;
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::Iterator::Iterator(
        const IndexIteratorType& index_iterator,
        const ValueIteratorType& value_iterator)
        : _index_iterator(index_iterator), _value_iterator(value_iterator)
    {
    }

    template <typename ElementType, typename IntegerListType>
    template <typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> Concept>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(IndexValueIteratorType indexValueIterator)
    {
        AppendElements(std::move(indexValueIterator));
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::initializer_list<IndexValue> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::initializer_list<double> list)
    {
        AppendElements(std::move(list));
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::vector<IndexValue> vec)
    {
        AppendElements(std::move(vec));
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::vector<double> vec)
    {
        AppendElements(std::move(vec));
    }

    template <typename ElementType, typename IntegerListType>
    typename SparseDataVector<ElementType, IntegerListType>::Iterator SparseDataVector<ElementType, IntegerListType>::GetIterator() const
    {
        return Iterator(_indices.GetIterator(), _values.cbegin());
    }

    template <typename ElementType, typename IntegerListType>
    void SparseDataVector<ElementType, IntegerListType>::AppendElement(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        ElementType storedValue = static_cast<ElementType>(value);
        assert(storedValue - value <= 1.0e-6 && value - storedValue <= 1.0e-6);

        if (_indices.Size() > 0)
        {
            if (index <= _indices.Max())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
            }
        }

        _indices.Append(index);
        _values.push_back(storedValue);
    }

    template <typename ElementType, typename IntegerListType>
    size_t SparseDataVector<ElementType, IntegerListType>::PrefixLength() const
    {
        if (_indices.Size() == 0)
        {
            return 0;
        }
        else
        {
            return _indices.Max() + 1;
        }
    }
}
}
