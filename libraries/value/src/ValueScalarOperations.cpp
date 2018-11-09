////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueScalarOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ValueScalarOperations.h"
#include "EmitterContext.h"
#include "ValueScalar.h"

namespace ell
{
namespace value
{

    Scalar operator+(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy += s2;
    }

    Scalar operator-(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy -= s2;
    }

    Scalar operator*(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy *= s2;
    }

    Scalar operator/(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy /= s2;
    }

    Scalar operator%(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy %= s2;
    }

    Scalar operator++(Scalar s) { return s += 1; }

    Scalar operator++(Scalar s, int)
    {
        Scalar copy = s.Copy();
        s += 1;
        return copy;
    }

    Scalar operator--(Scalar s) { return s -= 1; }

    Scalar operator--(Scalar s, int)
    {
        Scalar copy = s.Copy();
        s -= 1;
        return copy;
    }

} // namespace value
} // namespace ell