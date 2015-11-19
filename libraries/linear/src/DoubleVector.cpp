// DoubleVector.cpp

#include "DoubleVector.h"

namespace linear
{

    bool DoubleVector::Iterator::IsValid() const
    {
        return (_begin != _end);
    }

    void DoubleVector::Iterator::Next()
    {
        do
        {
            ++_begin;
            ++_index;
        } 
        while(_begin < _end && *_begin == 0);
    }

    indexValue DoubleVector::Iterator::Get() const
    {
        return indexValue{_index, (double)*_begin};
    }

    DoubleVector::Iterator::Iterator(const vector<double>::const_iterator& begin, const vector<double>::const_iterator& end) : _begin(begin), _end(end)
    {
        while(_begin < _end && *_begin == 0)
        {
            ++_begin;
            ++_index;
        }
    }

    void DoubleVector::Reset()
    {
        fill(begin(), end(), 0);
    }

    double DoubleVector::Norm2() const
    {
        double result = 0.0;
        for(double element : *this)
        {
            result += element * element;
        }
        return result;
    }

    void DoubleVector::AddTo(double* p_other, double scalar) const
    {
        for(uint64 i = 0; i<Size(); ++i)
        {
            p_other[i] += scalar * (*this)[i];
        }
    }

    void DoubleVector::Scale(double s)
    {
        for (uint64 i = 0; i<Size(); ++i)
        {
            (*this)[i] *= s;
        }
    }

    double DoubleVector::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint64 i = 0; i<Size(); ++i)
        {
            result += (*this)[i] * p_other[i];
        }
        
        return result;
    }

    uint64 DoubleVector::Size() const
    {
        return vector<double>::size();
    }

    void DoubleVector::Print(ostream & os) const
    {
        for (double x : *this)
        {
            os << x << '\t';
        }
    }

    DoubleVector::Iterator DoubleVector::GetIterator() const
    {
        return Iterator(cbegin(), cend());
    }
}