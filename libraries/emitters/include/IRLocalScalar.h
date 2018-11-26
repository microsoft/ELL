////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalScalar.h (emitters)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IREmitter.h"
#include "IRLocalValue.h"
#include "LLVMUtilities.h"

#include <utilities/include/TypeTraits.h>

namespace ell
{
namespace emitters
{
    /// <summary>
    /// Version of IRLocalValue for scalar values (values in registers)
    /// </summary>
    ///
    /// This subclass represents scalar values that can have arithmetic operations,
    /// comparisons, and simple math functions performed on them. The implementations
    /// for those operations are all separate functions and operator overloads.
    ///
    /// Usage:
    ///
    /// ```
    /// IRFunctionEmitter function = ...;
    /// LLVMValue outPtr = ...;
    /// LLVMValue value1 = ...;
    /// auto a = function.LocalScalar(value1);  // create an `IRLocalScalar` from an `LLVMValue`
    /// auto b = function.LocalScalar(1.25f);   // create an `IRLocalScalar` from a constant
    /// auto c = a + b;                         // directly perform math operations on `IRLocalScalar` values
    /// auto d = Sin(c);                        // ...and call math functions on them
    /// function.SetValue(outPtr, d);           // d implicitly converts to `LLVMValue` for functions that use llvm values directly
    /// ...
    /// ```
    struct IRLocalScalar : public IRLocalValue
    {
        using IRLocalValue::IRLocalValue;
        using IRLocalValue::operator=;

        bool IsConstantInt() const;
        bool IsConstantFloat() const;

        template <typename ValueType, utilities::IsIntegral<ValueType> = true>
        ValueType GetIntValue() const;

        template <typename ValueType, utilities::IsIntegral<ValueType> = true>
        ValueType GetIntValue(ValueType defaultValue) const;

        template <typename ValueType, utilities::IsFloatingPoint<ValueType> = true>
        ValueType GetFloatValue() const;

        template <typename ValueType, utilities::IsFloatingPoint<ValueType> = true>
        ValueType GetFloatValue(ValueType defaultValue) const;

        // Compound operators
        IRLocalScalar& operator+=(const IRLocalScalar& rhs);
        IRLocalScalar& operator-=(const IRLocalScalar& rhs);
        IRLocalScalar& operator*=(const IRLocalScalar& rhs);
        IRLocalScalar& operator/=(const IRLocalScalar& rhs);
        IRLocalScalar& operator%=(const IRLocalScalar& rhs);
        IRLocalScalar& operator|=(const IRLocalScalar& rhs);
        IRLocalScalar& operator&=(const IRLocalScalar& rhs);
        IRLocalScalar& operator^=(const IRLocalScalar& rhs);
        IRLocalScalar& operator<<=(const IRLocalScalar& rhs);
    };

    //
    // Convenience overloads for common operators
    //

    // Basic arithmetic
    IRLocalScalar operator+(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator+(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator+(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator+(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator+(IRLocalScalar a, ValueType b);

    IRLocalScalar operator-(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator-(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator-(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator-(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator-(IRLocalScalar a, ValueType b);

    IRLocalScalar operator-(IRLocalScalar a);
    IRLocalScalar operator*(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator*(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator*(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator*(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator*(IRLocalScalar a, ValueType b);

    IRLocalScalar operator/(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator/(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator/(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator/(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator/(IRLocalScalar a, ValueType b);

    IRLocalScalar operator%(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator%(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator%(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsIntegral<ValueType> = true>
    IRLocalScalar operator%(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsIntegral<ValueType> = true>
    IRLocalScalar operator%(IRLocalScalar a, ValueType b);

    // Bitwise operations
    IRLocalScalar operator&(IRLocalScalar a, IRLocalScalar b); // and
    IRLocalScalar operator|(IRLocalScalar a, IRLocalScalar b); // or
    IRLocalScalar operator^(IRLocalScalar a, IRLocalScalar b); // xor
    IRLocalScalar operator<<(IRLocalScalar a, IRLocalScalar b); // shift left

    // Logical operations
    IRLocalScalar operator&&(IRLocalScalar a, IRLocalScalar b); // logical and
    IRLocalScalar operator||(IRLocalScalar a, IRLocalScalar b); // logical or
    IRLocalScalar operator~(IRLocalScalar a); // logical not

    // Comparison operators
    IRLocalScalar operator==(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator==(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator==(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator==(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator==(IRLocalScalar a, ValueType b);

    IRLocalScalar operator!=(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator!=(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator!=(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator!=(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator!=(IRLocalScalar a, ValueType b);

    IRLocalScalar operator<(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator<(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator<(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator<(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator<(IRLocalScalar a, ValueType b);

    IRLocalScalar operator<=(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator<=(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator<=(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator<=(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator<=(IRLocalScalar a, ValueType b);

    IRLocalScalar operator>(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator>(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator>(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator>(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator>(IRLocalScalar a, ValueType b);

    IRLocalScalar operator>=(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator>=(LLVMValue a, IRLocalScalar b);
    IRLocalScalar operator>=(IRLocalScalar a, LLVMValue b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator>=(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator>=(IRLocalScalar a, ValueType b);

    // Common math functions
    IRLocalScalar Abs(IRLocalScalar a);
    IRLocalScalar Sqrt(IRLocalScalar a);
    IRLocalScalar Exp(IRLocalScalar a);
    IRLocalScalar Log(IRLocalScalar a);
    IRLocalScalar Sin(IRLocalScalar a);
    IRLocalScalar Cos(IRLocalScalar a);

    template <typename ValueType>
    IRLocalScalar Sigmoid(IRLocalScalar a);

    template <typename ValueType>
    IRLocalScalar Tanh(IRLocalScalar a);

    IRLocalScalar Min(IRLocalScalar a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar Min(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar Min(IRLocalScalar a, ValueType b);

    IRLocalScalar Max(IRLocalScalar a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar Max(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar Max(IRLocalScalar a, ValueType b);
} // namespace emitters
} // namespace ell

#pragma region implementation

namespace ell
{
namespace emitters
{
    namespace detail
    {
        IREmitter& GetEmitter(IRFunctionEmitter& function);

        template <typename ValueType>
        IRLocalScalar ToIRLocalScalar(IRFunctionEmitter& function, ValueType value)
        {
            return { function, GetEmitter(function).Literal(value) };
        }

        template <typename ValueType, utilities::IsSignedIntegral<ValueType> = true>
        ValueType GetConstantIntValue(llvm::ConstantInt* intValue)
        {
            return static_cast<ValueType>(intValue->getSExtValue());
        }

        template <typename ValueType, utilities::IsUnsignedIntegral<ValueType> = true>
        ValueType GetConstantIntValue(llvm::ConstantInt* intValue)
        {
            return static_cast<ValueType>(intValue->getZExtValue());
        }
    } // namespace detail

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    ValueType IRLocalScalar::GetIntValue() const
    {
        auto intValue = llvm::cast<llvm::ConstantInt>(this->value);
        return detail::GetConstantIntValue<ValueType>(intValue);
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    ValueType IRLocalScalar::GetIntValue(ValueType defaultValue) const
    {
        if (IsConstantInt())
        {
            return GetIntValue<ValueType>();
        }

        return defaultValue;
    }

    template <typename ValueType, utilities::IsFloatingPoint<ValueType> /* = true*/>
    ValueType IRLocalScalar::GetFloatValue(ValueType defaultValue) const
    {
        if (IsConstantFloat())
        {
            return GetFloatValue<ValueType>();
        }
        return defaultValue;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator+(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a + b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator+(IRLocalScalar a, ValueType value)
    {
        return value + a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator-(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a - b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator-(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a - b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator*(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a * b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator*(IRLocalScalar a, ValueType value)
    {
        return value * a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator/(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a / b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator/(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a / b;
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    IRLocalScalar operator%(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a % b;
    }

    template <typename ValueType, utilities::IsIntegral<ValueType> /* = true*/>
    IRLocalScalar operator%(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a % b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator==(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a == b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator==(IRLocalScalar a, ValueType value)
    {
        return value == a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator!=(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a != b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator!=(IRLocalScalar a, ValueType value)
    {
        return value != a;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a < b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a < b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<=(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a <= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator<=(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a <= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a > b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a > b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>=(ValueType value, IRLocalScalar b)
    {
        auto a = detail::ToIRLocalScalar(b.function, value);
        return a >= b;
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar operator>=(IRLocalScalar a, ValueType value)
    {
        auto b = detail::ToIRLocalScalar(a.function, value);
        return a >= b;
    }

    //
    // Math functions
    //
    template <typename ValueType>
    IRLocalScalar Sigmoid(IRLocalScalar a)
    {
        auto& fn = a.function;
        auto& emitter = detail::GetEmitter(fn);

        auto expInput = Exp(a);
        constexpr auto one = static_cast<ValueType>(1);
        auto result = emitter.Select(a > ValueType{ 0 }, one / (Exp(-a) + one), expInput / (expInput + one));
        return { fn, result };
    }

    template <typename ValueType>
    IRLocalScalar Tanh(IRLocalScalar a)
    {
        // tanh(x) === (exp(x) - exp(-x)) / (exp(x) + exp(-x))
        //         = 2*sigmoid(2*x) - 1
        auto two = static_cast<ValueType>(2.0);
        return (two * Sigmoid<ValueType>(two * a)) - static_cast<ValueType>(1);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Min(ValueType value, IRLocalScalar b)
    {
        return Min(detail::ToIRLocalScalar(b.function, value), b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Min(IRLocalScalar a, ValueType value)
    {
        return Min(value, a);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Max(ValueType value, IRLocalScalar b)
    {
        return Max(detail::ToIRLocalScalar(b.function, value), b);
    }

    template <typename ValueType, utilities::IsFundamental<ValueType> /* = true*/>
    IRLocalScalar Max(IRLocalScalar a, ValueType value)
    {
        return Max(value, a);
    }
} // namespace emitters
} // namespace ell

#pragma endregion implementation
