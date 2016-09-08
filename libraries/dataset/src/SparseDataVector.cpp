////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SparseDataVector.h"

namespace dataset
{
    template <typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::Iterator::Next()
    {
        _index_iterator.Next();
        ++_value_iterator;
    }

    template <typename ValueType, typename IntegerListType>
    SparseDataVector<ValueType, IntegerListType>::Iterator::Iterator(
        const IndexIteratorType& index_iterator,
        const ValueIteratorType& value_iterator)
        : _index_iterator(index_iterator), _value_iterator(value_iterator)
    {
    }

    template <typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::AppendEntry(uint64_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        _indices.Append(index);
        _values.push_back((ValueType)value);
    }

    template <typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::Reset()
    {
        _indices.Reset();
        _values.resize(0);
    }

    template <typename ValueType, typename IntegerListType>
    uint64_t SparseDataVector<ValueType, IntegerListType>::Size() const
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

    template <typename ValueType, typename IntegerListType>
    double SparseDataVector<ValueType, IntegerListType>::Norm2() const
    {
        double result = 0.0;
        for (auto value : _values)
        {
            result += value * value;
        }
        return result;
    }

    template <typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::AddTo(double* p_other, double scalar) const
    {
        auto iter = GetIterator();
        while (iter.IsValid())
        {
            auto current = iter.Get();
            p_other[current.index] += scalar * current.value;
            iter.Next();
        }
    }

    template <typename ValueType, typename IntegerListType>
    double SparseDataVector<ValueType, IntegerListType>::Dot(const double* p_other) const
    {
        double value = 0.0;

        auto iter = GetIterator();
        while (iter.IsValid())
        {
            auto current = iter.Get();
            value += current.value * p_other[current.index];
            iter.Next();
        }

        return value;
    }

    template <typename ValueType, typename IntegerListType>
    typename SparseDataVector<ValueType, IntegerListType>::Iterator SparseDataVector<ValueType, IntegerListType>::GetIterator() const
    {
        return Iterator(_indices.GetIterator(), _values.cbegin());
    }

    template <typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::Print(std::ostream& os) const
    {
        auto iterator = GetIterator();
        while (iterator.IsValid())
        {
            auto entry = iterator.Get();
            os << entry.index << ':' << entry.value << '\t';
            iterator.Next();
        }
    }

    template class SparseDataVector<double, utilities::CompressedIntegerList>;
    template class SparseDataVector<float, utilities::CompressedIntegerList>;
    template class SparseDataVector<short, utilities::CompressedIntegerList>;
}
