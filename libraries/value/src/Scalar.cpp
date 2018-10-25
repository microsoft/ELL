////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Scalar.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Scalar.h"
#include "EmitterContext.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace value
{
using namespace utilities;

Scalar::Scalar() = default;

Scalar::Scalar(Value value) : _value(std::move(value))
{
    if (!_value.IsDefined() || !_value.IsConstrained() || _value.GetLayout().NumDimensions() != 0)
    {
        throw InputException(InputExceptionErrors::invalidArgument);
    }
}

Scalar::operator Value() const { return _value; }

Scalar Scalar::Copy() const
{
    Scalar s(Allocate(GetType(), MemoryLayout{}));
    s = *this;
    return s;
}

ValueType Scalar::GetType() const
{
    return _value.GetBaseType();
}

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

} // value
} // ell
