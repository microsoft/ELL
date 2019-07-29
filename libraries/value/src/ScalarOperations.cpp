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

    Scalar operator-(Scalar s)
    {
        return Cast(0, s.GetType()) - s;
    }

    Scalar operator++(Scalar s)
    {
        if (!s.GetValue().IsIntegral())
        {
            throw LogicException(LogicExceptionErrors::illegalState);
        }

        return s += Cast(1, s.GetType());
    }

    Scalar operator++(Scalar s, int)
    {
        if (!s.GetValue().IsIntegral())
        {
            throw LogicException(LogicExceptionErrors::illegalState);
        }

        Scalar copy = s.Copy();
        s += Cast(1, s.GetType());
        return copy;
    }

    Scalar operator--(Scalar s)
    {
        if (!s.GetValue().IsIntegral())
        {
            throw LogicException(LogicExceptionErrors::illegalState);
        }

        return s -= Cast(1, s.GetType());
    }

    Scalar operator--(Scalar s, int)
    {
        if (!s.GetValue().IsIntegral())
        {
            throw LogicException(LogicExceptionErrors::illegalState);
        }

        Scalar copy = s.Copy();
        s -= Cast(1, s.GetType());
        return copy;
    }

    Scalar operator==(Scalar s1, Scalar s2)
    {
        return GetContext().LogicalOperation(ValueLogicalOperation::equality, s1.GetValue(), s2.GetValue());
    }

    Scalar operator!=(Scalar s1, Scalar s2)
    {
        return GetContext().LogicalOperation(ValueLogicalOperation::inequality, s1.GetValue(), s2.GetValue());
    }

    Scalar operator<=(Scalar s1, Scalar s2)
    {
        return GetContext().LogicalOperation(ValueLogicalOperation::lessthanorequal, s1.GetValue(), s2.GetValue());
    }

    Scalar operator<(Scalar s1, Scalar s2)
    {
        return GetContext().LogicalOperation(ValueLogicalOperation::lessthan, s1.GetValue(), s2.GetValue());
    }

    Scalar operator>=(Scalar s1, Scalar s2)
    {
        return GetContext().LogicalOperation(ValueLogicalOperation::greaterthanorequal, s1.GetValue(), s2.GetValue());
    }

    Scalar operator>(Scalar s1, Scalar s2)
    {
        return GetContext().LogicalOperation(ValueLogicalOperation::greaterthan, s1.GetValue(), s2.GetValue());
    }

    Scalar operator&&(Scalar s1, Scalar s2)
    {
        return GetContext().BinaryOperation(ValueBinaryOperation::logicalAnd, s1.GetValue(), s2.GetValue());
    }

    Scalar operator||(Scalar s1, Scalar s2)
    {
        return GetContext().BinaryOperation(ValueBinaryOperation::logicalOr, s1.GetValue(), s2.GetValue());
    }

} // namespace value
} // namespace ell
