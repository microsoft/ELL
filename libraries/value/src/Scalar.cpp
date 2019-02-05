////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Scalar.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Scalar.h"
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

    Scalar::~Scalar() = default;
    Scalar::Scalar(const Scalar&) = default;
    Scalar::Scalar(Scalar&&) noexcept = default;

    Scalar& Scalar::operator=(const Scalar& other)
    {
        if (this != &other)
        {
            _value = other._value;
        }
        return *this;
    }

    Scalar& Scalar::operator=(Scalar&& other)
    {
        if (this != &other)
        {
            _value = std::move(other._value);
            other._value = Value();
        }
        return *this;
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
