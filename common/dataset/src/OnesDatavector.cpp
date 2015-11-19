// OnesDataVector.cpp

#include "OnesDataVector.h"

#include "types.h"
using linear::uint;

#include <stdexcept>
using std::runtime_error;

#include <cmath>
using std::fabs;

#include <cassert>

#include <memory>
using std::move;

namespace dataset
{
    OnesDataVector::OnesDataVector(uint dim) : _dim(dim)
    {}

    OnesDataVector::OnesDataVector(const IDataVector& other) : _dim(other.Size())
    {}

    IDataVector::type OnesDataVector::GetType() const
    {
        return type::ones;
    }

    void OnesDataVector::PushBack(uint index, double value)
    {
        if(value != 1 || index != _dim)
        {
            throw runtime_error("Cannot set elements of a ones vector to non-one values");
        }
        ++_dim;
    }

    void OnesDataVector::Reset()
    {
        _dim = 0;
    }

    //void OnesDataVector::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const // TODO
    //{
    //    for(uint i=0; i<_dim; ++i)
    //    {
    //        func(index_offset+i, 1.0);
    //    }
    //}

    uint OnesDataVector::Size() const
    {
        return _dim;
    }

    uint OnesDataVector::NumNonzeros() const
    {
        return _dim;
    }

    double OnesDataVector::Norm2() const
    {
        return (double)_dim;
    }

    void OnesDataVector::AddTo(double* p_other, double scalar) const
    {
        for(uint i = 0; i<_dim; ++i)
        {
            p_other[i] += (double)scalar;
        }
    }

    double OnesDataVector::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint i = 0; i<_dim; ++i)
        {
            result += p_other[i];
        }

        return result;
    }

    void OnesDataVector::Print(ostream & os) const
    {
        for(int i=0; i<_dim; ++i)
        {
            os << i << ":1\t";
        }
    }
}