// SparseBinaryDataVector.cpp

#include "SparseBinaryDataVector.h"

#include <cassert>

namespace dataset
{
    template<typename IntegerListType>
    bool SparseBinaryDataVectorBase<IntegerListType>::Iterator::IsValid() const
    {
        return _list_iterator.IsValid();
    }

    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::Iterator::Next()
    {
        _list_iterator.Next();
    }

    template<typename IntegerListType>
    uint64 SparseBinaryDataVectorBase<IntegerListType>::Iterator::GetIndex() const
    {
        return _list_iterator.Get();
    }

    template<typename IntegerListType>
    double SparseBinaryDataVectorBase<IntegerListType>::Iterator::Get() const
    {
        return (double)1.0;
    }

    template<typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::Iterator::Iterator(const IndexIteratorType& list_iterator)
        : _list_iterator(list_iterator)
    {}

    template<typename IntegerListType>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase() 
    {}

    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::PushBack(uint64 index, double value)
    {
        if (value == 0)
        {
            return;
        }

        assert(value == 1);

        _indices.PushBack(index);
    }

    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::Reset()
    {
        _indices.Reset();
    }

    template<typename IntegerListType>
    uint64 SparseBinaryDataVectorBase<IntegerListType>::Size() const
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

    template<typename IntegerListType>
    uint64 SparseBinaryDataVectorBase<IntegerListType>::NumNonzeros() const
    {
        return _indices.Size();
    }

    template<typename IntegerListType>
    double SparseBinaryDataVectorBase<IntegerListType>::Norm2() const
    {
        return (double)_indices.Size();
    }
    
    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::AddTo(double* p_other, double scalar) const
    {
        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            p_other[iter.Get()] += scalar;
            iter.Next();
        }
    }

    template<typename IntegerListType>
    double SparseBinaryDataVectorBase<IntegerListType>::Dot(const double* p_other) const
    {
        double value = 0.0;
        
        auto iter = _indices.GetIterator();
        while (iter.IsValid())
        {
            value += (double)p_other[iter.Get()];
            iter.Next();
        }
        
        return value;
    }

    template<typename IntegerListType>
    typename SparseBinaryDataVectorBase<IntegerListType>::Iterator SparseBinaryDataVectorBase<IntegerListType>::GetIterator() const
    {
        return Iterator(_indices.GetIterator());
    }

    template<typename IntegerListType>
    void SparseBinaryDataVectorBase<IntegerListType>::Print(ostream & os) const
    {
        auto iterator = GetIterator();
        while(iterator.IsValid())
        {
            auto IndexValue = iterator.Get();
            os << IndexValue << '\t';
            iterator.Next();
        }
    }

    IDataVector::type SparseBinaryDataVector::GetType() const
    {
        return type::sparse_binary;
    }

    IDataVector::type UncompressedSparseBinaryVector::GetType() const
    {
        return type::uncompressed_sparse_binary;
    }

    // Explicit instantiation
    template class SparseBinaryDataVectorBase<CompressedIntegerList>;
    template class SparseBinaryDataVectorBase<IntegerList>;
}
