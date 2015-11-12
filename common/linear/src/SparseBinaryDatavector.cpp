// SparseBinaryDatavector.cpp

#include "SparseBinaryDatavector.h"
#include <cassert>

namespace linear
{
    template<typename IntegerListType>
    bool SparseBinaryDatavectorBase<IntegerListType>::ConstIterator::IsValid() const
    {
        return _list_iterator.IsValid();
    }

    template<typename IntegerListType>
    void SparseBinaryDatavectorBase<IntegerListType>::ConstIterator::Next()
    {
        _list_iterator.Next();
    }

    template<typename IntegerListType>
    uint SparseBinaryDatavectorBase<IntegerListType>::ConstIterator::GetIndex() const
    {
        return _list_iterator.GetValue();
    }

    template<typename IntegerListType>
    double SparseBinaryDatavectorBase<IntegerListType>::ConstIterator::GetValue() const
    {
        return (double)1.0;
    }

    template<typename IntegerListType>
    SparseBinaryDatavectorBase<IntegerListType>::ConstIterator::ConstIterator(const index_iter_type& list_iterator)
        : _list_iterator(list_iterator)
    {}

    template<typename IntegerListType>
    SparseBinaryDatavectorBase<IntegerListType>::SparseBinaryDatavectorBase() 
    {}

    template<typename IntegerListType>
    SparseBinaryDatavectorBase<IntegerListType>::SparseBinaryDatavectorBase(const IDataVector& other) 
    {
        //_indices.Reserve(other.NumNonzeros());

        //other.foreach_nonzero(
        //[this](uint index, double value) 
        //{ 
        //    if(value > 0)
        //    {
        //        this->PushBack(index);
        //    }
        //});

        // TODO
    }

    template<typename IntegerListType>
    void SparseBinaryDatavectorBase<IntegerListType>::PushBack(uint index, double value)
    {
        if (value == 0)
        {
            return;
        }

        assert(value == 1);

        _indices.PushBack(index);
    }

    template<typename IntegerListType>
    void SparseBinaryDatavectorBase<IntegerListType>::Reset()
    {
        _indices.Reset();
    }

    //template<typename IntegerListType>
    //void SparseBinaryDatavectorBase<IntegerListType>::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const
    //{
    //    auto iter = _indices.GetConstIterator();
    //    while(iter.IsValid())
    //    {
    //        func(index_offset + iter.GetValue(), (double)1.0);
    //        iter.Next();
    //    }
    //}

    template<typename IntegerListType>
    uint SparseBinaryDatavectorBase<IntegerListType>::Size() const
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
    uint SparseBinaryDatavectorBase<IntegerListType>::NumNonzeros() const
    {
        return _indices.Size();
    }

    template<typename IntegerListType>
    double SparseBinaryDatavectorBase<IntegerListType>::Norm2() const
    {
        return (double)_indices.Size();
    }
    
    template<typename IntegerListType>
    void SparseBinaryDatavectorBase<IntegerListType>::AddTo(double* p_other, double scalar) const
    {
        auto iter = _indices.GetConstIterator();
        while (iter.IsValid())
        {
            p_other[iter.GetValue()] += scalar;
            iter.Next();
        }
    }

    template<typename IntegerListType>
    double SparseBinaryDatavectorBase<IntegerListType>::Dot(const double* p_other) const
    {
        double value = 0.0;
        
        auto iter = _indices.GetConstIterator();
        while (iter.IsValid())
        {
            value += (double)p_other[iter.GetValue()];
            iter.Next();
        }
        
        return value;
    }

    template<typename IntegerListType>
    typename SparseBinaryDatavectorBase<IntegerListType>::ConstIterator SparseBinaryDatavectorBase<IntegerListType>::GetConstIterator() const
    {
        return ConstIterator(_indices.GetConstIterator());
    }

    template SparseBinaryDatavectorBase<CompressedIntegerList>;

    IDataVector::type SparseBinaryDatavector::GetType() const
    {
        return type::sparse_binary;
    }

    template SparseBinaryDatavectorBase<IntegerList>;

    IDataVector::type UncompressedSparseBinaryVector::GetType() const
    {
        return type::uncompressed_sparse_binary;
    }
}