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
    bool OnesDataVector::Iterator::IsValid() const
    {
        return (_index < _size);
    }

    void OnesDataVector::Iterator::Next()
    {
        ++_index;
    }

    IndexValue OnesDataVector::Iterator::GetValue() const
    {
        return IndexValue(_index, 1);
    }

    OnesDataVector::Iterator::Iterator(uint size) : _size(size)
    {}

    OnesDataVector::OnesDataVector(uint dim) : _size(dim)
    {}

    OnesDataVector::OnesDataVector(const IDataVector& other) : _size(other.Size())
    {}

    IDataVector::type OnesDataVector::GetType() const
    {
        return type::ones;
    }

    void OnesDataVector::PushBack(uint index, double value)
    {
        if(value != 1 || index != _size)
        {
            throw runtime_error("Cannot set elements of a ones vector to non-one values");
        }
        ++_size;
    }

    void OnesDataVector::Reset()
    {
        _size = 0;
    }

    //void OnesDataVector::foreach_nonzero(function<void(uint, double)> func, uint index_offset) const // TODO
    //{
    //    for(uint i=0; i<_size; ++i)
    //    {
    //        func(index_offset+i, 1.0);
    //    }
    //}

    uint OnesDataVector::Size() const
    {
        return _size;
    }

    uint OnesDataVector::NumNonzeros() const
    {
        return _size;
    }

    double OnesDataVector::Norm2() const
    {
        return (double)_size;
    }

    void OnesDataVector::AddTo(double* p_other, double scalar) const
    {
        for(uint i = 0; i<_size; ++i)
        {
            p_other[i] += (double)scalar;
        }
    }

    double OnesDataVector::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint i = 0; i<_size; ++i)
        {
            result += p_other[i];
        }

        return result;
    }

    OnesDataVector::Iterator OnesDataVector::GetIterator() const
    {
        return Iterator(_size);
    }

    void OnesDataVector::Print(ostream & os) const
    {
        for(int i=0; i<_size; ++i)
        {
            os << i << ":1\t";
        }
    }
}