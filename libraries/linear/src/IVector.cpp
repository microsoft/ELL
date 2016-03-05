////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     IVector.cpp (linear)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IVector.h"
#include "DoubleVector.h"

// stl
#include <cassert>

namespace linear
{
    void IVector::AddTo(std::vector<double>& other, double scalar) const
    {
        assert(Size() <= other.size());
        if (Size() > 0)
        {
            AddTo(other.data(), scalar);
        }
    }

    void IVector::AddTo(types::RealArray<double>& other, double scalar) const
    {
        assert(Size() <= other.size());
        if (Size() > 0)
        {
            AddTo(other.GetDataPointer(), scalar);
        }
    }

    void IVector::AddTo(DoubleVector& other, double scalar) const
    {
        assert(Size() <= other.size());
        if (Size() > 0)
        {
            AddTo(other.GetDataPointer(), scalar);
        }
    }

    double IVector::Dot(const std::vector<double>& other) const
    {
        assert(Size() <= other.size());
        if (Size() == 0)
        {
            return 0;
        }

        return Dot(other.data());
    }

    double IVector::Dot(const types::RealArray<double>& other) const
    {
        assert(Size() <= other.size());
        if (Size() == 0)
        {
            return 0;
        }

        return Dot(other.GetDataPointer());
    }

    double IVector::Dot(const DoubleVector& other) const
    {
        assert(Size() <= other.size());
        if (Size() == 0)
        {
            return 0;
        }

        return Dot(other.GetDataPointer());
    }
}
