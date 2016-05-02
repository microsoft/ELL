////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ZeroDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ZeroDataVector.h"

// stl
#include <stdexcept>

namespace dataset
{

    bool ZeroDataVector::Iterator::IsValid() const
    {
        return false;
    }

    void ZeroDataVector::Iterator::Next()
    {}

    linear::IndexValue ZeroDataVector::Iterator::Get() const
    {
        throw std::runtime_error("this code should bever be reached");
    }

    IDataVector::type ZeroDataVector::GetType() const
    {
        return type::zero;
    }

    void ZeroDataVector::AppendEntry(uint64_t index, double value)
    {
        if(value != 0)
        {
            throw std::runtime_error("Cannot set elements of a zero vector");
        }
    }

    void ZeroDataVector::Reset()
    {}

    uint64_t ZeroDataVector::Size() const
    {
        return 0;
    }

    uint64_t ZeroDataVector::NumNonzeros() const
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

    ZeroDataVector::Iterator ZeroDataVector::GetIterator() const
    {
        return Iterator();
    }

    void ZeroDataVector::Print(std::ostream & os) const
    {}

    std::unique_ptr<IDataVector> ZeroDataVector::Clone() const
    {
        auto ptr = std::make_unique<ZeroDataVector>(*this); 
        return std::move(ptr);
    }
}
