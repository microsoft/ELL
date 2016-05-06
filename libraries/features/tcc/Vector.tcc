////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Vector.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cmath>

namespace features
{
    template <typename T>
    double Vector<T>::LengthSquared()
    {
        double lenSq = 0;
        for (auto x : _data)
        {
            lenSq += x*x;
        }
        return lenSq;
    }

    template <typename T>
    double Vector<T>::Length()
    {
        return std::sqrt(LengthSquared());
    }
}
