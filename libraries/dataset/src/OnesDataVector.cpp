////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     OnesDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OnesDataVector.h"

// stl
#include <stdexcept>
#include <cmath>

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

    types::IndexValue OnesDataVector::Iterator::Get() const
    {
        return types::IndexValue{_index, 1};
    }

    OnesDataVector::Iterator::Iterator(uint64 size) : _size(size)
    {}

    OnesDataVector::OnesDataVector(uint64 dim) : _size(dim)
    {}

    OnesDataVector::OnesDataVector(const IDataVector& other) : _size(other.Size())
    {}

    IDataVector::type OnesDataVector::GetType() const
    {
        return type::ones;
    }

    void OnesDataVector::PushBack(uint64 index, double value)
    {
        if(value != 1 || index != _size)
        {
            throw std::runtime_error("Cannot set elements of a ones vector to non-one values");
        }
        ++_size;
    }

    void OnesDataVector::Reset()
    {
        _size = 0;
    }

    uint64 OnesDataVector::Size() const
    {
        return _size;
    }

    uint64 OnesDataVector::NumNonzeros() const
    {
        return _size;
    }

    double OnesDataVector::Norm2() const
    {
        return (double)_size;
    }

    void OnesDataVector::AddTo(double* p_other, double scalar) const
    {
        for(uint64 i = 0; i<_size; ++i)
        {
            p_other[i] += (double)scalar;
        }
    }

    double OnesDataVector::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint64 i = 0; i<_size; ++i)
        {
            result += p_other[i];
        }

        return result;
    }

    OnesDataVector::Iterator OnesDataVector::GetIterator() const
    {
        return Iterator(_size);
    }

    void OnesDataVector::Print(std::ostream & os) const
    {
        for(int i=0; i<_size; ++i)
        {
            os << i << ":1\t";
        }
    }
}
