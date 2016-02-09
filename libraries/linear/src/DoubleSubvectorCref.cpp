////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DoubleSubvectorCref.cpp (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DoubleSubvectorCref.h"

namespace linear
{
    DoubleSubvectorCref::DoubleSubvectorCref(const DoubleVector& vec, uint64 offset, uint64 size) : _ptr(&vec[offset]), _size(size)
    {
        if (_size > vec.Size())
        {
            _size = vec.Size();
        }
    }

    DoubleSubvectorCref::DoubleSubvectorCref(const double * ptr, uint64 size) : _ptr(ptr), _size(size)
    {}

    double DoubleSubvectorCref::Norm2() const
    {
        double result = 0.0;
        for(uint64 i=0; i<_size; ++i)
        {
            double x = _ptr[i];
            result += x * x;
        }
        return result;
    }

    void DoubleSubvectorCref::AddTo(double* p_other, double scalar) const
    {
        for(uint64 i = 0; i<_size; ++i)
        {
            p_other[i] += scalar * _ptr[i];
        }
    }


    double DoubleSubvectorCref::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint64 i = 0; i<_size; ++i)
        {
            result += _ptr[i] * p_other[i];
        }
        
        return result;
    }

    void DoubleSubvectorCref::Print(ostream & os) const
    {
        for(uint64 i = 0; i<_size; ++i)
        {
            os << _ptr[i] << '\t';
        }
    }

    uint64 DoubleSubvectorCref::Size() const
    {
        return _size;
    }
}
