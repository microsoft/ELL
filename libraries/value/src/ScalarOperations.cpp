////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalarOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ScalarOperations.h"
#include "EmitterContext.h"
#include "Scalar.h"

#include <emitters/include/IREmitter.h>

namespace ell
{
using namespace utilities;

namespace value
{

    Scalar Add(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy += s2;
    }

    Scalar Subtract(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy -= s2;
    }

    Scalar Multiply(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy *= s2;
    }

    Scalar Divide(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy /= s2;
    }

    Scalar Modulo(Scalar s1, Scalar s2)
    {
        Scalar copy = s1.Copy();
        return copy %= s2;
    }

    Scalar FusedMultiplyAdd(Scalar a, Scalar b, Scalar c)
    {
        return Fma(a, b, c);
    }

} // namespace value
} // namespace ell
