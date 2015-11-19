// ZeroDataVector.cpp

#include "ZeroDataVector.h"

#include <stdexcept>

using std::runtime_error;

namespace dataset
{
    ZeroDataVector::ZeroDataVector(const IDataVector& other) 
    {}

    IDataVector::type ZeroDataVector::GetType() const
    {
        return type::zero;
    }

    void ZeroDataVector::PushBack(uint index, double value)
    {
        if(value != 0)
        {
            throw runtime_error("Cannot set elements of a zero vector");
        }
    }

    void ZeroDataVector::Reset()
    {}

    //void ZeroDataVector::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const // TODO
    //{}

    uint ZeroDataVector::Size() const
    {
        return 0;
    }

    uint ZeroDataVector::NumNonzeros() const
    {
        return 0;
    }

    double ZeroDataVector::Norm2() const
    {
        return 0.0;
    }

    void ZeroDataVector::AddTo(double* p_other, double scalar) const
    {}

    double ZeroDataVector::Dot(const double* p_other) const
    {
        return 0.0;
    }

    void ZeroDataVector::Print(ostream & os) const
    {}
}