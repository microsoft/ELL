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
    template <typename IndexValueIteratorType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(IndexValueIteratorType indexValueIterator, linear::IsIndexValueIterator<IndexValueIteratorType>)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            SparseDataVector<ElementType, IntegerListType>::AppendElement(indexValue.index, indexValue.value); // explicit call to SparseDataVector<ElementType, IntegerListType>::AppendElement is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template<typename ElementType, typename IntegerListType>
    template<typename DataVectorType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(DataVectorType dataVector, typename IsDataVector<DataVectorType>) : SparseDataVector(dataVector.GetIterator())
    {
        static_assert(std::is_same<DataVectorType, SparseDataVector<ElementType, IntegerListType>>::value == false, "Move ctor should have been called instead");
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::initializer_list<linear::IndexValue> list)
    {
        auto current = list.begin();
        auto end = list.end();
        while(current < end)
        {
            SparseDataVector<ElementType, IntegerListType>::AppendElement(current->index, current->value); // explicit call to SparseDataVector<ElementType, IntegerListType>::AppendElement is given to avoid virtual function call in Ctor
            ++current;
        }
    }

    template <typename ElementType, typename IntegerListType>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::initializer_list<double> list)
    {
        auto current = list.begin();
        auto end = list.end();
        size_t index = 0;
        while(current < end)
        {
            SparseDataVector<ElementType, IntegerListType>::AppendElement(index, *current); // explicit call to SparseDataVector<ElementType, IntegerListType>::AppendElement is given to avoid virtual function call in Ctor
            ++current;
            ++index;
        }
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

        if(_indices.Size() > 0)
        {
            if(index <= _indices.Max())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
            }
        }

        _indices.Append(index);
        _values.push_back(storedValue);
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
