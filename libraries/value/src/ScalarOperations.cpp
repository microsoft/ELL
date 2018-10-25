////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalarOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ScalarOperations.h"
#include "Scalar.h"

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

}
}