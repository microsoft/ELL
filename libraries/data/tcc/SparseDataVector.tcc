////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseDataVector.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
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
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::initializer_list<IndexValue> list) // TODO change this as above, to use AppendElements (also in denseDV)
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
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(std::initializer_list<double> list) // TODO change this to use AppendElements (also in denseDV)
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
    size_t SparseDataVector<ElementType, IntegerListType>::ZeroSuffixFirstIndex() const
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
