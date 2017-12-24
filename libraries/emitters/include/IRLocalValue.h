////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValue.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "TypeTraits.h"

// llvm
#include "llvm/IR/Value.h"

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;

    /// <summary>
    /// Helper type for llvm values representing values local to a function
    /// </summary>
    struct IRLocalValue
    {
        IRLocalValue() = default;
        IRLocalValue(const IRLocalValue&) = default;

        /// <summary> Constructor from an llvm::Value* </summary>
        ///
        /// <param name="function"> The current function being emitted. </param>
        /// <param name="value"> The LLVM value to wrap. </param>
        ///
        /// <returns> An `IRLocalValue` wrapping the given value. </returns>
        IRLocalValue(IRFunctionEmitter& function, llvm::Value* value);

        /// <summary> Constructor for a literal values of fundamental types </summary>
        ///
        /// <param name="function"> The current function being emitted. </param>
        /// <param name="value"> The LLVM value to wrap. </param>
        ///
        /// <returns> An `IRLocalValue` wrapping the given value. </returns>
        template <typename ValueType, utilities::IsFundamental<ValueType> = true>
        IRLocalValue(IRFunctionEmitter& function, ValueType value);

        /// <summary> Implicit conversion to llvm::Value* </summary>
        ///
        /// <returns> An `llvm::Value` pointer to the wrapped value. </returns>
        operator llvm::Value*() { return value; }

        /// <summary> The function this value is in scope for. </summary>
        IRFunctionEmitter& function;

        /// <summary> The llvm::Value* being wrapped. </summary>
        llvm::Value* value;
    };

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
    };

    /// <summary>
    /// Version of IRLocalValue for pointer values
    /// </summary>
    struct IRLocalPointer : public IRLocalValue
    {
        using IRLocalValue::IRLocalValue;
    };

    //
    // Convenience overloads for common operators
    //
    // Basic arithmetic
    IRLocalScalar operator+(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator-(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator-(IRLocalScalar a);
    IRLocalScalar operator*(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator/(IRLocalScalar a, IRLocalScalar b);

    // Logical operations
    IRLocalScalar operator&&(IRLocalScalar a, IRLocalScalar b); // logical and
    IRLocalScalar operator||(IRLocalScalar a, IRLocalScalar b); // logical or
    IRLocalScalar operator~(IRLocalScalar a); // logical not

    // Comparison operators
    IRLocalScalar operator==(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator!=(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator<(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator<=(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator>(IRLocalScalar a, IRLocalScalar b);
    IRLocalScalar operator>=(IRLocalScalar a, IRLocalScalar b);

    // Common math functions
    IRLocalScalar Abs(IRLocalScalar a);
    IRLocalScalar Sqrt(IRLocalScalar a);
    IRLocalScalar Exp(IRLocalScalar a);
    IRLocalScalar Log(IRLocalScalar a);
    IRLocalScalar Sin(IRLocalScalar a);
    IRLocalScalar Cos(IRLocalScalar a);
}
}
