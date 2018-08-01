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

// utilities
#include "TypeTraits.h"

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
    /// llvm::Value* outPtr = ...;
    /// llvm::Value* value1 = ...;
    /// auto a = function.LocalScalar(value1);  // create an `IRLocalScalar` from an `llvm::Value*`
    /// auto b = function.LocalScalar(1.25f);   // create an `IRLocalScalar` from a constant
    /// auto c = a + b;                         // directly perform math operations on `IRLocalScalar` values
    /// auto d = Sin(c);                        // ...and call math functions on them
    /// function.SetValue(outPtr, d);           // d implicitly converts to `llvm::Value*` for functions that use llvm values directly
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
    IRLocalScalar operator+(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator+(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator+(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator+(IRLocalScalar a, ValueType b);

    IRLocalScalar operator-(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator-(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator-(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator-(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator-(IRLocalScalar a, ValueType b);

    IRLocalScalar operator-(IRLocalScalar a);
    IRLocalScalar operator*(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator*(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator*(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator*(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator*(IRLocalScalar a, ValueType b);

    IRLocalScalar operator/(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator/(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator/(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator/(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator/(IRLocalScalar a, ValueType b);

    IRLocalScalar operator%(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator%(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator%(IRLocalScalar a, llvm::Value* b);
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
    IRLocalScalar operator==(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator==(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator==(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator==(IRLocalScalar a, ValueType b);

    IRLocalScalar operator!=(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator!=(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator!=(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator!=(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator!=(IRLocalScalar a, ValueType b);

    IRLocalScalar operator<(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator<(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator<(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator<(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator<(IRLocalScalar a, ValueType b);

    IRLocalScalar operator<=(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator<=(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator<=(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator<=(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator<=(IRLocalScalar a, ValueType b);

    IRLocalScalar operator>(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator>(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator>(IRLocalScalar a, llvm::Value* b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator>(ValueType a, IRLocalScalar b);
    template <typename ValueType, utilities::IsFundamental<ValueType> = true>
    IRLocalScalar operator>(IRLocalScalar a, ValueType b);

    IRLocalScalar operator>=(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator>=(llvm::Value* a, IRLocalScalar b);
    IRLocalScalar operator>=(IRLocalScalar a, llvm::Value* b);
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
}
}

#include "../tcc/IRLocalScalar.tcc"
