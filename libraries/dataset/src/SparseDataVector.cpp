////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SparseDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SparseDataVector.h"

namespace dataset
{
    template<typename ValueType, typename IntegerListType>
    bool SparseDataVector<ValueType, IntegerListType>::Iterator::IsValid() const
    {
        return _index_iterator.IsValid();
    }

    template<typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::Iterator::Next()
    {
        _index_iterator.Next();
        ++_value_iterator;
    }

    template<typename ValueType, typename IntegerListType>
    types::IndexValue SparseDataVector<ValueType, IntegerListType>::Iterator::Get() const
    {
        return types::IndexValue{_index_iterator.Get(), (double)*_value_iterator};
    }

    template<typename ValueType, typename IntegerListType>
    SparseDataVector<ValueType, IntegerListType>::Iterator::Iterator(
        const IndexIteratorType& index_iterator,
        const ValueIteratorType& value_iterator)
        : _index_iterator(index_iterator), _value_iterator(value_iterator)
    {}

    template<typename ValueType, typename IntegerListType>
    SparseDataVector<ValueType, IntegerListType>::SparseDataVector() 
    {}

    template<typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::PushBack(uint64_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        _indices.PushBack(index);
        _values.push_back((ValueType)value);
    }

    template<typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::Reset()
    {
        _indices.Reset();
        _values.resize(0);
    }

    template<typename ValueType, typename IntegerListType>
    uint64_t SparseDataVector<ValueType, IntegerListType>::Size() const
    {
        if(_indices.Size() == 0)
        {
            return 0;
        }
        else
        {
            return _indices.Max() + 1;
        }
    }

    template<typename ValueType, typename IntegerListType>
    uint64_t SparseDataVector<ValueType, IntegerListType>::NumNonzeros() const
    {
        return _indices.Size();
    }
    
    template<typename ValueType, typename IntegerListType>
    double SparseDataVector<ValueType, IntegerListType>::Norm2() const
    {
        return (double)_indices.Size();
    }
    
    template<typename ValueType, typename IntegerListType>
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

    template<typename ValueType, typename IntegerListType>
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

    template<typename ValueType, typename IntegerListType>
    typename SparseDataVector<ValueType, IntegerListType>::Iterator SparseDataVector<ValueType, IntegerListType>::GetIterator() const
    {
        return Iterator(_indices.GetIterator(), _values.cbegin());
    }

    template<typename ValueType, typename IntegerListType>
    void SparseDataVector<ValueType, IntegerListType>::Print(std::ostream & os) const
    {
        auto iterator = GetIterator();
        while(iterator.IsValid())
        {
            auto entry = iterator.Get();
            os << entry.index << ':' << entry.value << '\t';
            iterator.Next();
        }
    }

    template class SparseDataVector<double, utilities::CompressedIntegerList>;

    IDataVector::type SparseDoubleDataVector::GetType() const
    {
        return type::sparse_double;
    }

    template class SparseDataVector<float, utilities::CompressedIntegerList>;

    IDataVector::type SparseFloatDataVector::GetType() const
    {
        return type::sparse_float;
    }

    template class SparseDataVector<short, utilities::CompressedIntegerList>;

    IDataVector::type SparseShortDataVector::GetType() const
    {
        return type::sparse_short;
    }
}
