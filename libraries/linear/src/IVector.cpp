////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
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

    double IVector::Dot(const std::vector<double>& other) const
    {
        assert(Size() <= other.size());
        if (Size() == 0)
        {
            return 0;
        }

        return Dot(other.data());
    }

}
