////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Value.h"
#include "EmitterContext.h"
#include "Scalar.h"

#include <utilities/include/Hash.h>

namespace ell
{
namespace value
{
    using namespace detail;
    using namespace utilities;

    Value::Value() = default;

    Value::~Value() = default;

    Value::Value(const Value&) = default;

    Value::Value(Value&& other) noexcept = default;

    // clang-format off
    /****************************************************************************
    Copy assignment has to be context aware. The terms "defined", "empty", and
    "constrained" match their definitions of the respective Value member functions.
    Shallow copies of data is done in this function. Deep copies are done by the
    global EmitterContext object.
    * if lhs is not defined
      * and rhs is not defined: don't do anything
      * and rhs is defined: copy everything
    * if lhs is defined and not constrained and empty
      * and rhs is not defined: throw
      * and rhs is defined: if types don't match, throw. copy everything
    * if lhs is defined and constrained and empty
      * and rhs is not defined: throw
      * and rhs is defined and not constrained and empty: if types don't match, throw.
      * and rhs is defined and constrained and empty: if types or layout don't match, throw
      * and rhs is defined and not constrained and not empty: if types don't match, throw. shallow copy of data
      * and rhs is defined and constrained and not empty: if types or layout don't match, throw. shallow copy of data
    * if lhs is defined and not constrained and not empty:
      * throw
    * if lhs is defined and constrained and not empty:
      * and rhs does not match type or layout: throw
      * and rhs matches type and layout: deep copy of data
    ****************************************************************************/
    // clang-format on
    Value& Value::operator=(const Value& other)
    {
        if (this != &other)
        {
            if (!IsDefined())
            {
                // lhs is not defined
                if (other.IsDefined())
                {
                    _layout = other._layout;
                    _data = other._data;
                    _type = other._type;
                }
            }
            else
            {
                // lhs is defined
                if (!other.IsDefined())
                {
                    throw InputException(InputExceptionErrors::invalidArgument);
                }
                if (GetBaseType() != other.GetBaseType())
                {
                    throw InputException(InputExceptionErrors::typeMismatch);
                }

                if (!IsConstrained())
                {
                    // lhs is not constrained
                    if (IsEmpty())
                    {
                        _layout = other._layout;
                        _data = other._data;
                        _type = other._type;
                    }
                    else
                    {
                        throw LogicException(LogicExceptionErrors::illegalState);
                    }
                }
                else
                {
                    // lhs is constrained
                    if (IsEmpty())
                    {
                        if (other.IsConstrained() && GetLayout() != other.GetLayout())
                        {
                            throw InputException(InputExceptionErrors::sizeMismatch);
                        }
                        _data = other._data;
                        _type = other._type;
                    }
                    else
                    {
                        if (!other.IsConstrained() || GetLayout() != other.GetLayout())
                        {
                            throw InputException(InputExceptionErrors::sizeMismatch);
                        }
                        GetContext().CopyData(other, *this);
                    }
                }
            }
        }

        return *this;
    }

    // Behaves similarly to the copy assignment, except rhs is reset afterwards
    Value& Value::operator=(Value&& other)
    {
        if (this != &other)
        {
            if (!IsDefined())
            {
                // lhs is not defined
                if (other.IsDefined())
                {
                    _data = std::move(other._data);
                    _layout = std::move(other._layout);
                    _type = std::move(other._type);
                }
            }
            else
            {
                // lhs is defined
                if (!other.IsDefined())
                {
                    throw InputException(InputExceptionErrors::invalidArgument);
                }
                if (GetBaseType() != other.GetBaseType())
                {
                    throw InputException(InputExceptionErrors::typeMismatch);
                }

                if (!IsConstrained())
                {
                    // lhs is not constrained
                    if (IsEmpty())
                    {
                        _data = std::move(other._data);
                        _layout = std::move(other._layout);
                        _type = std::move(other._type);
                    }
                    else
                    {
                        throw LogicException(LogicExceptionErrors::illegalState);
                    }
                }
                else
                {
                    // lhs is constrained
                    if (IsEmpty())
                    {
                        if (other.IsConstrained() && GetLayout() != other.GetLayout())
                        {
                            throw InputException(InputExceptionErrors::sizeMismatch);
                        }
                        _data = std::move(other._data);
                        _type = other._type;
                    }
                    else
                    {
                        if (!other.IsConstrained() || GetLayout() != other.GetLayout())
                        {
                            throw InputException(InputExceptionErrors::sizeMismatch);
                        }
                        GetContext().MoveData(other, *this);
                    }
                }
            }
            other.Reset();
        }

        return *this;
    }

    Value::Value(Emittable data, std::optional<MemoryLayout> layout) :
        _data(data),
        _type(GetContext().GetType(data)),
        _layout(layout)
    {}

    Value::Value(ValueType type, std::optional<MemoryLayout> layout) :
        _type({ type, 1 }),
        _layout(layout)
    {}

    Value::Value(detail::ValueTypeDescription typeDescription, std::optional<MemoryLayout> layout) noexcept :
        _type(typeDescription),
        _layout(layout)
    {}

    void Value::Reset()
    {
        _type = { ValueType::Undefined, 0 };
        _layout.reset();
        _data = {};
    }

    void Value::SetData(Value value)
    {
        if (value.IsConstrained() && value.GetLayout() != GetLayout())
        {
            throw InputException(InputExceptionErrors::invalidArgument);
        }

        std::visit(VariantVisitor{ [](Undefined) { throw InputException(InputExceptionErrors::invalidArgument); },
                                   [this](Emittable emittable) {
                                       auto type = GetContext().GetType(emittable);
                                       if (type.first != _type.first)
                                       {
                                           throw InputException(InputExceptionErrors::typeMismatch);
                                       }

                                       _data = emittable;
                                       _type = type;
                                   },
                                   [this, &value](auto&& arg) {
                                       if (GetValueType<std::decay_t<decltype(arg)>>() != _type.first)
                                       {
                                           throw InputException(InputExceptionErrors::typeMismatch);
                                       }

                                       _data = arg;
                                       _type = value._type;
                                   } },
                   value._data);
    }

    bool Value::IsDefined() const { return _type.first != ValueType::Undefined; }

    bool Value::IsEmpty() const { return std::holds_alternative<Undefined>(_data); }

    bool Value::IsConstant() const { return !std::holds_alternative<Emittable>(_data); }

    bool Value::IsIntegral() const
    {
        if (IsPointer())
        {
            return false;
        }

        switch (_type.first)
        {
        case ValueType::Boolean: // [[fallthrough]];
        case ValueType::Byte: // [[fallthrough]];
        case ValueType::Char8: // [[fallthrough]];
        case ValueType::Int16: // [[fallthrough]];
        case ValueType::Int32: // [[fallthrough]];
        case ValueType::Int64:
            return true;
        default:
            return false;
        }
    }

    bool Value::IsBoolean() const { return _type.first == ValueType::Boolean && !IsPointer(); }

    bool Value::IsInt16() const { return _type.first == ValueType::Int16 && !IsPointer(); }

    bool Value::IsInt32() const { return _type.first == ValueType::Int32 && !IsPointer(); }

    bool Value::IsInt64() const { return _type.first == ValueType::Int64 && !IsPointer(); }

    bool Value::IsFloatingPoint() const
    {
        return (_type.first == ValueType::Float || _type.first == ValueType::Double) && !IsPointer();
    }

    bool Value::IsFloat32() const { return _type.first == ValueType::Float && !IsPointer(); }

    bool Value::IsDouble() const { return _type.first == ValueType::Double && !IsPointer(); }

    bool Value::IsPointer() const { return _type.second > 1; }

    bool Value::IsIntegralPointer() const
    {
        if (!IsPointer())
        {
            return false;
        }

        switch (_type.first)
        {
        case ValueType::Boolean: // [[fallthrough]];
        case ValueType::Byte: // [[fallthrough]];
        case ValueType::Int16: // [[fallthrough]];
        case ValueType::Char8: // [[fallthrough]];
        case ValueType::Int32: // [[fallthrough]];
        case ValueType::Int64:
            return true;
        default:
            return false;
        }
    }

    bool Value::IsBooleanPointer() const { return _type.first == ValueType::Boolean && IsPointer(); }

    bool Value::IsShortPointer() const { return _type.first == ValueType::Int16 && IsPointer(); }

    bool Value::IsInt32Pointer() const { return _type.first == ValueType::Int32 && IsPointer(); }

    bool Value::IsInt64Pointer() const { return _type.first == ValueType::Int64 && IsPointer(); }

    bool Value::IsFloatingPointPointer() const
    {
        return (_type.first == ValueType::Float || _type.first == ValueType::Double) && IsPointer();
    }

    bool Value::IsFloat32Pointer() const { return _type.first == ValueType::Float && IsPointer(); }

    bool Value::IsDoublePointer() const { return _type.first == ValueType::Double && IsPointer(); }

    bool Value::IsConstrained() const { return _layout.has_value(); }

    const MemoryLayout& Value::GetLayout() const { return _layout.value(); }

    Value Value::Offset(Value index) const { return GetContext().Offset(*this, index); }

    Value Value::Offset(Scalar index) const { return Offset(index.GetValue()); }

    ValueType Value::GetBaseType() const { return _type.first; }

    void Value::SetLayout(MemoryLayout layout) { _layout = layout; }

    void Value::ClearLayout() { _layout.reset(); }

    int Value::PointerLevel() const { return _type.second; }

    Value::UnderlyingDataType& Value::GetUnderlyingData() { return _data; }

    const Value::UnderlyingDataType& Value::GetUnderlyingData() const { return _data; }

    namespace detail
    {
        Value StoreConstantData(ConstantData data) { return GetContext().StoreConstantData(data); }
    } // namespace detail

} // namespace value
} // namespace ell

size_t std::hash<::ell::value::Value>::operator()(const ::ell::value::Value& value) const noexcept
{
    using ::ell::utilities::HashCombine;

    size_t hash = 0;
    HashCombine(hash, value.GetBaseType());
    HashCombine(hash, value.PointerLevel());

    if (value.IsConstrained())
    {
        HashCombine(hash, value.GetLayout());
    }
    else
    {
        // Special random value for an unconstrained Value
        HashCombine(hash, 0x87654321);
    }

    return hash;
}
