////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     DoubleSubvector.cpp (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DoubleSubvector.h"

namespace linear
{
    DoubleSubvector::DoubleSubvector(const DoubleVector& vec, uint64_t offset, uint64_t size) : _ptr(vec.GetDataPointer() + offset), _size(size)
    {
        if (_size > vec.Size())
        {
            _size = vec.Size();
        }
    }

    DoubleSubvector::DoubleSubvector(const double * ptr, uint64_t size) : _ptr(ptr), _size(size)
    {}

    DoubleSubvector::Iterator DoubleSubvector::GetIterator() const
    {
        return Iterator(_ptr, _ptr+_size);
    }

    uint64_t DoubleSubvector::Size() const
    {
        return _size;
    }

    double DoubleSubvector::Norm2() const
    {
        double result = 0.0;
        for (uint64_t i = 0; i < _size; ++i)
        {
            double x = _ptr[i];
            result += x * x;
        }
        return result;
    }

    void DoubleSubvector::AddTo(double* p_other, double scalar) const
    {
        for (uint64_t i = 0; i < _size; ++i)
        {
            p_other[i] += scalar * _ptr[i];
        }
    }


    double DoubleSubvector::Dot(const double* p_other) const
    {
        double result = 0.0;
        for (uint64_t i = 0; i < _size; ++i)
        {
            result += _ptr[i] * p_other[i];
        }

        return result;
    }

    void DoubleSubvector::Print(std::ostream & os) const
    {
        auto iterator = GetIterator();
        while (iterator.IsValid())
        {
            auto indexValue = iterator.Get();
            os << indexValue.index << ':' << indexValue.value << '\t';
            iterator.Next();
        }
    }
}
