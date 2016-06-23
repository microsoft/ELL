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

// utilities
#include "Exception.h"

namespace dataset
{
    OnesDataVector::Iterator::Iterator(uint64_t size) : _size(size)
    {}

    OnesDataVector::OnesDataVector(uint64_t dim) : _size(dim)
    {}

    void OnesDataVector::AppendEntry(uint64_t index, double value)
    {
        if(value != 1 || index != _size)
        {
            throw utilities::Exception(utilities::ExceptionErrorCodes::invalidArgument, "Cannot set elements of a ones vector to non-one values");
        }
        ++_size;
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

    void OnesDataVector::Print(std::ostream & os) const
    {
        for(int i=0; i<_size; ++i)
        {
            os << i << ":1\t";
        }
    }

    std::vector<double> OnesDataVector::ToArray() const
    {
        auto vector = std::vector<double>(Size());
        std::fill(vector.begin(), vector.end(), 1.0);
        return vector;
    }

}
