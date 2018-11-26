////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueScalar.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ValueScalar.h"
#include "EmitterContext.h"

#include <utilities/include/Exception.h>

namespace ell
{
namespace value
{
    using namespace utilities;

    Scalar::Scalar() = default;

    Scalar::Scalar(Value value) :
        _value(std::move(value))
    {
        if (!_value.IsDefined() || !_value.IsConstrained() || _value.GetLayout().NumDimensions() != 0)
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }
    }

    Value Scalar::GetValue() const { return _value; }

    Scalar Scalar::Copy() const
    {
        Scalar s(Allocate(GetType(), MemoryLayout{}));
        s = *this;
        return s;
    }

    ValueType Scalar::GetType() const { return _value.GetBaseType(); }

    Scalar& Scalar::operator+=(Scalar s)
    {
        _value = GetContext().BinaryOperation(ValueBinaryOperation::add, _value, s._value);
        return *this;
    }

    Scalar& Scalar::operator-=(Scalar s)
    {
        _value = GetContext().BinaryOperation(ValueBinaryOperation::subtract, _value, s._value);
        return *this;
    }

    Scalar& Scalar::operator*=(Scalar s)
    {
        _value = GetContext().BinaryOperation(ValueBinaryOperation::multiply, _value, s._value);
        return *this;
    }

    Scalar& Scalar::operator/=(Scalar s)
    {
        _value = GetContext().BinaryOperation(ValueBinaryOperation::divide, _value, s._value);
        return *this;
    }

    Scalar& Scalar::operator%=(Scalar s)
    {
        _value = GetContext().BinaryOperation(ValueBinaryOperation::modulus, _value, s._value);
        return *this;
    }

} // namespace value
} // namespace ell
