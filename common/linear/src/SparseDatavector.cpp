// SparseDatavector.cpp

#include "SparseDatavector.h"

namespace linear
{
    template<typename ValueType, typename IntegerListType>
    bool SparseDatavector<ValueType, IntegerListType>::Iterator::IsValid() const
    {
        return _index_iterator.IsValid();
    }

    template<typename ValueType, typename IntegerListType>
    void SparseDatavector<ValueType, IntegerListType>::Iterator::Next()
    {
        _index_iterator.Next();
        ++_value_iterator;
    }

    template<typename ValueType, typename IntegerListType>
    IndexValue SparseDatavector<ValueType, IntegerListType>::Iterator::GetValue() const
    {
        return IndexValue(_index_iterator.GetValue(), *_value_iterator);
    }

    template<typename ValueType, typename IntegerListType>
    SparseDatavector<ValueType, IntegerListType>::Iterator::Iterator(
        const IndexIteratorType& index_iterator,
        const ValueIteratorType& value_iterator)
        : _index_iterator(index_iterator), _value_iterator(value_iterator)
    {}

    template<typename ValueType, typename IntegerListType>
    SparseDatavector<ValueType, IntegerListType>::SparseDatavector() 
    {}

    template<typename ValueType, typename IntegerListType>
    SparseDatavector<ValueType, IntegerListType>::SparseDatavector(const IDataVector& other) 
    {
        //_indices.Reserve(other.NumNonzeros());
        //_values.Reserve(other.NumNonzeros());

        //other.foreach_nonzero(
        //[this](uint index, double value) 
        //{ 
        //    this->PushBack(index, value);
        //});

        // TODO
    }

    template<typename ValueType, typename IntegerListType>
    void SparseDatavector<ValueType, IntegerListType>::PushBack(uint index, double value)
    {
        if (value == 0)
        {
            return;
        }

        _indices.PushBack(index);
        _values.push_back((ValueType)value);
    }

    template<typename ValueType, typename IntegerListType>
    void SparseDatavector<ValueType, IntegerListType>::Reset()
    {
        _indices.Reset();
        _values.resize(0);
    }

    //template<typename ValueType, typename IntegerListType>
    //void SparseDatavector<ValueType, IntegerListType>::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const
    //{
    //    auto iter = GetIterator();
    //    while(iter.IsValid())
    //    {
    //        func(index_offset + iter.GetIndex(), iter.GetValue());
    //        iter.Next();
    //    }
    //}

    template<typename ValueType, typename IntegerListType>
    uint SparseDatavector<ValueType, IntegerListType>::Size() const
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
    uint SparseDatavector<ValueType, IntegerListType>::NumNonzeros() const
    {
        return _indices.Size();
    }
    
    template<typename ValueType, typename IntegerListType>
    double SparseDatavector<ValueType, IntegerListType>::Norm2() const
    {
        return (double)_indices.Size();
    }
    
    template<typename ValueType, typename IntegerListType>
    void SparseDatavector<ValueType, IntegerListType>::AddTo(double* p_other, double scalar) const
    {
        auto iter = GetIterator();
        while (iter.IsValid())
        {
            auto current = iter.GetValue();
            p_other[current.GetIndex()] += scalar * current.GetValue();
            iter.Next();
        }
    }

    template<typename ValueType, typename IntegerListType>
    double SparseDatavector<ValueType, IntegerListType>::Dot(const double* p_other) const
    {
        double value = 0.0;
        
        auto iter = GetIterator();
        while (iter.IsValid())
        {
            auto current = iter.GetValue();
            value += current.GetValue() * p_other[current.GetIndex()];
            iter.Next();
        }
    
        return value;
    }

    template<typename ValueType, typename IntegerListType>
    typename SparseDatavector<ValueType, IntegerListType>::Iterator SparseDatavector<ValueType, IntegerListType>::GetIterator() const
    {
        return Iterator(_indices.GetIterator(), _values.cbegin());
    }

    template<typename ValueType, typename IntegerListType>
    void SparseDatavector<ValueType, IntegerListType>::Print(ostream & os) const
    {
        auto iterator = GetIterator();
        while(iterator.IsValid())
        {
            auto indexValue = iterator.GetValue();
            os << indexValue << '\t';
            iterator.Next();
        }
    }

    template SparseDatavector<double, CompressedIntegerList>;

    IDataVector::type SparseDoubleDatavector::GetType() const
    {
        return type::sparse_double;
    }

    template SparseDatavector<float, CompressedIntegerList>;

    IDataVector::type SparseFloatDatavector::GetType() const
    {
        return type::sparse_float;
    }

    template SparseDatavector<short, CompressedIntegerList>;

    IDataVector::type SparseShortDatavector::GetType() const
    {
        return type::sparse_short;
    }
}