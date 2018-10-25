////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector.h"
#include "EmitterContext.h"

namespace ell
{
namespace value
{
using namespace utilities;

Vector::Vector() = default;

Vector::Vector(Value value) : _value(value)
{
    if (!_value.IsDefined() || !_value.IsConstrained() || _value.GetLayout().NumDimensions() != 1)
    {
        throw InputException(InputExceptionErrors::invalidArgument, "Value passed in must be one-dimensional");
    }
}

Scalar Vector::operator[](Scalar index) { return (*this)(index); }

Scalar Vector::operator()(Scalar index)
{
    Value indexedValue = GetContext().Offset(_value, { index });
    indexedValue.SetLayout(MemoryLayout{});

    return indexedValue;
}

Vector::operator Value() const { return _value; }

Vector Vector::SubVector(Scalar offset, int size) const
{
    Value indexedValue = GetContext().Offset(_value, { offset });
    const auto& layout = _value.GetLayout();
    auto newShape = layout.GetActiveSize();
    newShape[0] = size;
    indexedValue.SetLayout(
        MemoryLayout(newShape, layout.GetStride(), layout.GetOffset(), layout.GetLogicalDimensionOrder())
    );

    return indexedValue;
}

Vector Vector::Copy() const
{
    auto newValue = Allocate(_value.GetBaseType(), _value.GetLayout());
    newValue = _value;
    return newValue;
}

size_t Vector::Size() const { return _value.GetLayout().NumElements(); }

ValueType Vector::GetType() const { return _value.GetBaseType(); }

} // value
} // ell
