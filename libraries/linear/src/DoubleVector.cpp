////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DoubleVector.cpp (linear)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DoubleVector.h"

// stl
#include <algorithm>

namespace linear
{
    DoubleVector::DoubleVector(uint64_t size) : _data(size)
    {}

    void DoubleVector::Reset()
    {
        std::fill(_data.begin(), _data.end(), 0);
    }

    double DoubleVector::Norm2() const
    {
        double result = 0.0;
        for(double element : _data)
        {
            result += element * element;
        }
        return result;
    }

    void DoubleVector::AddTo(double* p_other, double scalar) const
    {
        for(uint64_t i = 0; i<Size(); ++i)
        {
            p_other[i] += scalar * _data[i];
        }
    }

    void DoubleVector::Scale(double s)
    {
        for (uint64_t i = 0; i<Size(); ++i)
        {
            (*this)[i] *= s;
        }
    }

    double DoubleVector::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint64_t i = 0; i<Size(); ++i)
        {
            result += (*this)[i] * p_other[i];
        }
        
        return result;
    }

    DoubleVector::Iterator DoubleVector::GetIterator() const
    {
        return utilities::MakeStlIndexValueIterator(_data);
    }
    
    void DoubleVector::Print(std::ostream & os) const
    {
        auto iterator = GetIterator();
        while (iterator.IsValid())
        {
            auto indexValue = iterator.Get();
            os << indexValue.index << ':' << indexValue.value << '\t';
            iterator.Next();
        }
    }

    void DoubleVector::Swap(DoubleVector& u, DoubleVector& v)
    {
        std::swap(u._data, v._data);
    }

}
