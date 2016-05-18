////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ZeroDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZeroDataVector.h"

// stl
#include <stdexcept>

namespace dataset
{
    linear::IndexValue ZeroDataVector::Iterator::Get() const
    {
        throw std::runtime_error("this code should bever be reached");
    }

    void ZeroDataVector::AppendEntry(uint64_t index, double value)
    {
        if(value != 0)
        {
            throw std::runtime_error("Cannot set elements of a zero vector");
        }
    }

    void ZeroDataVector::AddTo(double* p_other, double scalar) const
    {}

    double ZeroDataVector::Dot(const double* p_other) const
    {
        return 0.0;
    }

    void ZeroDataVector::Print(std::ostream & os) const
    {}
}
