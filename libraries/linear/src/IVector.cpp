// IVector.cpp

#include "IVector.h"

#include <cassert>

namespace linear
{
    void IVector::AddTo(std::vector<double>& other, double scalar) const
    {
        assert(Size() <= other.size());
        if(Size() > 0)
        {
            AddTo(&other[0], scalar);
        }
    }

    double IVector::Dot(const std::vector<double>& other) const
    {
        assert(Size() <= other.size());
        if(Size() == 0)
        {
            return 0;
        }

        return Dot(&other[0]);
    }
}
