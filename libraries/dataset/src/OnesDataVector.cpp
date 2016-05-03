////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OnesDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
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

    linear::IndexValue OnesDataVector::Iterator::Get() const
    {
        return linear::IndexValue{_index, 1};
    }

    OnesDataVector::Iterator::Iterator(uint64_t size) : _size(size)
    {}

    OnesDataVector::OnesDataVector(uint64_t dim) : _size(dim)
    {}

    IDataVector::type OnesDataVector::GetType() const
    {
        return type::ones;
    }

    void OnesDataVector::AppendEntry(uint64_t index, double value)
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

    uint64_t OnesDataVector::Size() const
    {
        return _size;
    }

    uint64_t OnesDataVector::NumNonzeros() const
    {
        return _size;
    }

    double OnesDataVector::Norm2() const
    {
        return (double)_size;
    }

    void OnesDataVector::AddTo(double* p_other, double scalar) const
    {
        for(uint64_t i = 0; i<_size; ++i)
        {
            p_other[i] += (double)scalar;
        }
    }

    double OnesDataVector::Dot(const double* p_other) const
    {
        double result = 0.0;
        for(uint64_t i = 0; i<_size; ++i)
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

    std::unique_ptr<IDataVector> OnesDataVector::Clone() const
    {
        auto ptr = std::make_unique<OnesDataVector>(*this); 
        return std::move(ptr);
    }

    std::vector<double> OnesDataVector::ToArray() const
    {
        auto vector = std::vector<double>(Size());
        std::fill(vector.begin(), vector.end(), 1.0);
        return vector;
    }

}
