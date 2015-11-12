// DenseDatavector.cpp

#include "DenseDatavector.h"

#include <cassert>

namespace linear
{
    template<typename ValueType>
    bool DenseDatavector<ValueType>::Iterator::IsValid() const
    {
        return (_begin == _end);
    }

    template<typename ValueType>
    void DenseDatavector<ValueType>::Iterator::Next()
    {
        do
        {
            ++_begin;
            ++_index;
        } 
        while(_begin < _end && *_begin == 0);
    }

    template<typename ValueType>
    IndexValue DenseDatavector<ValueType>::Iterator::GetValue() const
    {
        return IndexValue(_index, (double)*_begin);
    }

    template<typename ValueType>
    DenseDatavector<ValueType>::Iterator::Iterator(const StlIteratorType& begin, const StlIteratorType& end) : _begin(begin), _end(end)
    {}

    template<typename ValueType>
    DenseDatavector<ValueType>::DenseDatavector()
    {
        _mem.reserve(DEFAULT_DENSE_VECTOR_CAPACITY);
    }

    template<typename ValueType>
    DenseDatavector<ValueType>::DenseDatavector(const IDataVector& other) : _mem(other.Size()), _num_nonzeros(other.NumNonzeros())
    {
        //auto setter = [this](uint index, double value)
        //{
        //    _mem[index] = (ValueType)value;

        //};
        //other.foreach_nonzero(setter);

        // TODO
    }

    template<typename ValueType>
    void DenseDatavector<ValueType>::PushBack(uint index, double value)
    {
        if(value == 0)
        {
            return;
        }
        
        assert(index >= Size());

        _mem.resize(index+1);
        _mem[index] = (ValueType)value;
        ++_num_nonzeros;
    }
    
    template<typename ValueType>
    void DenseDatavector<ValueType>::Reset()
    {
        _mem.resize(0);
        _num_nonzeros = 0;
    }

    //template<typename ValueType>
    //void DenseDatavector<ValueType>::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const // TODO
    //{
    //    for(uint i=0; i<Size(); ++i)
    //    {
    //        double value = _mem[i];
    //        if(value != 0)
    //        {
    //            func(index_offset + i, value);
    //        }
    //    }
    //}

    template<typename ValueType>
    uint DenseDatavector<ValueType>::Size() const
    {
        return _mem.size();
    }

    template<typename ValueType>
    uint DenseDatavector<ValueType>::NumNonzeros() const
    {
        return _num_nonzeros;
    }

    template<typename ValueType>
    double DenseDatavector<ValueType>::Norm2() const
    {
        double result = 0.0;
        for(double element : _mem)
        {
            result += (double)(element * element);
        }
        return result;
    }

    template<typename ValueType>
    void DenseDatavector<ValueType>::AddTo(double* p_other, double scalar) const
    {
        for(uint i = 0; i<Size(); ++i)
        {
            p_other[i] += (double)(scalar * _mem[i]);
        }
    }

    template<typename ValueType>
    double DenseDatavector<ValueType>::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint i = 0; i<Size(); ++i)
        {
            result += _mem[i] * p_other[i];
        }
        
        return result;
    }

    template<typename ValueType>
    typename DenseDatavector<ValueType>::Iterator DenseDatavector<ValueType>::GetIterator() const
    {
        return Iterator(_mem.begin(), _mem.end());
    }

    template<typename ValueType>
    void DenseDatavector<ValueType>::Print(ostream & os) const
    {
        auto iterator =  GetIterator();
        while(iterator.IsValid())
        {
            auto indexValue = iterator.GetValue();
            os << indexValue << '\t';
            iterator.Next();
        }
    }

    template DenseDatavector<float>;

    IDataVector::type FloatDatavector::GetType() const
    {
        return type::dense_double;
    }

    template DenseDatavector<double>;

    IDataVector::type DoubleDatavector::GetType() const
    {
        return type::dense_float;
    }
}