////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     DenseDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DenseDataVector.h"

// stl
#include <cassert>

namespace dataset
{
    template<typename ValueType>
    DenseDataVector<ValueType>::DenseDataVector() : _num_nonzeros(0)
    {
        _data.reserve(DEFAULT_DENSE_VECTOR_CAPACITY);
    }

    template<typename ValueType>
    void DenseDataVector<ValueType>::AppendEntry(uint64_t index, double value)
    {
        if(value == 0)
        {
            return;
        }
        
        assert(index >= Size());

        _data.resize(index+1);
        _data[index] = (ValueType)value;
        ++_num_nonzeros;
    }
    
    template<typename ValueType>
    void DenseDataVector<ValueType>::Reset()
    {
        _data.resize(0);
        _num_nonzeros = 0;
    }

    template<typename ValueType>
    uint64_t DenseDataVector<ValueType>::Size() const
    {
        return _data.size();
    }

    template<typename ValueType>
    uint64_t DenseDataVector<ValueType>::NumNonzeros() const
    {
        return _num_nonzeros;
    }

    template<typename ValueType>
    double DenseDataVector<ValueType>::Norm2() const
    {
        double result = 0.0;
        for(double element : _data)
        {
            result += (double)(element * element);
        }
        return result;
    }

    template<typename ValueType>
    void DenseDataVector<ValueType>::AddTo(double* p_other, double scalar) const
    {
        for(uint64_t i = 0; i<Size(); ++i)
        {
            p_other[i] += (double)(scalar * _data[i]);
        }
    }

    template<typename ValueType>
    double DenseDataVector<ValueType>::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint64_t i = 0; i<Size(); ++i)
        {
            result += _data[i] * p_other[i];
        }
        
        return result;
    }

    template<typename ValueType>
    typename DenseDataVector<ValueType>::Iterator DenseDataVector<ValueType>::GetIterator() const
    {
        return utilities::MakeStlIndexValueIterator(_data);
    }

    template<typename ValueType>
    void DenseDataVector<ValueType>::Print(std::ostream & os) const
    {
        auto iterator = GetIterator();
        while (iterator.IsValid())
        {
            auto indexValue = iterator.Get();
            os << indexValue.index << ':' << indexValue.value << '\t';
            iterator.Next();
        }
    }

    template class DenseDataVector<float>;

    IDataVector::type FloatDataVector::GetType() const
    {
        return type::dense_double;
    }

    template class DenseDataVector<double>;

    IDataVector::type DoubleDataVector::GetType() const
    {
        return type::dense_float;
    }
}
