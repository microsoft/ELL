////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalarOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <type_traits>

namespace ell
{
namespace value
{

    class Scalar;

    /// <summary> Arithmetic operators </summary>
    Scalar Add(Scalar, Scalar);
    Scalar Subtract(Scalar, Scalar);
    Scalar Multiply(Scalar, Scalar);
    Scalar Divide(Scalar, Scalar);
    Scalar Modulo(Scalar, Scalar);
    Scalar FusedMultiplyAdd(Scalar a, Scalar b, Scalar c); // returns (a*b)+c

} // namespace value
} // namespace ell
