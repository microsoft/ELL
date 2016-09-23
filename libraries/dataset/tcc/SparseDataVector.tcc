////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace dataset
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
    template <typename IndexValueIteratorType, typename concept>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            SparseDataVector<ElementType, IntegerListType>::AppendEntry(indexValue.index, indexValue.value); // explicit call to SparseDataVector<ElementType, IntegerListType>::AppendEntry is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::initializer_list<linear::IndexValue> list)
    {
        auto current = list.begin();
        auto end = list.end();
        while(current < end)
        {
            SparseDataVector<ElementType, IntegerListType>::AppendEntry(current->index, current->value); // explicit call to SparseDataVector<ElementType, IntegerListType>::AppendEntry is given to avoid virtual function call in Ctor
            ++current;
        }
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::initializer_list<size_t> list)
    {
        auto current = list.begin();
        auto end = list.end();
        while(current < end)
        {
            SparseDataVector<ElementType, IntegerListType>::AppendEntry(*current, 1.0); // explicit call to SparseDataVector<ElementType, IntegerListType>::AppendEntry is given to avoid virtual function call in Ctor
            ++current;
        }
    }

    template <typename ElementType, typename IntegerListType>
    typename SparseDataVector<ElementType, IntegerListType>::Iterator SparseDataVector<ElementType, IntegerListType>::GetIterator() const
    {
        return Iterator(_indices.GetIterator(), _values.cbegin());
    }

    template <typename ElementType, typename IntegerListType>
    void SparseDataVector<ElementType, IntegerListType>::AppendEntry(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        _indices.Append(index);
        _values.push_back((ElementType)value);
    }

    template <typename ElementType, typename IntegerListType>
    size_t SparseDataVector<ElementType, IntegerListType>::Size() const
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
