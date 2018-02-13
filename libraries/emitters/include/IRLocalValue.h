////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValue.h (emitters)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "TypeTraits.h"

// llvm
#include <llvm/IR/Value.h>

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;

    /// <summary>
    /// Abstract base class for helper types for llvm values representing values local to a function
    /// </summary>
    struct IRLocalValue
    {
        IRLocalValue(const IRLocalValue&) = default;
        IRLocalValue(IRLocalValue&&) = default;
        IRLocalValue& operator=(const IRLocalValue& other);
        IRLocalValue& operator=(llvm::Value* value);

        /// <summary> Constructor from an llvm::Value* </summary>
        ///
        /// <param name="function"> The current function being emitted. </param>
        /// <param name="value"> The LLVM value to wrap. </param>
        ///
        /// <returns> An `IRLocalValue` wrapping the given value. </returns>
        IRLocalValue(IRFunctionEmitter& function, llvm::Value* value);

        /// <summary> Implicit conversion to llvm::Value* </summary>
        ///
        /// <returns> An `llvm::Value` pointer to the wrapped value. </returns>
        operator llvm::Value*() const { return value; }

        /// <summary> The function this value is in scope for. </summary>
        IRFunctionEmitter& function;

        /// <summary> The llvm::Value* being wrapped. </summary>
        llvm::Value* value;

    private:
        IRLocalValue() = default;
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
        using IRLocalValue::operator=;
    };

    /// <summary>
    /// Version of IRLocalValue for pointer values
    /// </summary>
    struct IRLocalPointer : public IRLocalValue
    {
        using IRLocalValue::IRLocalValue;
    };

    /// Helper type for llvm values representing array values local to a function
    /// </summary>
    struct IRLocalArray : public IRLocalValue
    {
    private:
        /// <summary>
        /// Helper type for llvm values representing values within arrays local to a function
        /// </summary>
        struct IRLocalArrayValue
        {
            IRLocalArrayValue(IRFunctionEmitter& function, llvm::Value* pPointer, llvm::Value* pOffset);

            IRLocalArrayValue& operator=(llvm::Value* value);

            operator IRLocalScalar() const;

            IRFunctionEmitter& _function;
            llvm::Value* _pPointer;
            llvm::Value* _pOffset;
        };

    public:
        using IRLocalValue::IRLocalValue;

        /// <summary> Indexing operator to return a reference to the specified offset </summary>
        ///
        /// <param name="offset"> The offset where the value lies within the wrapped array. </param>
        /// <return> An instance of IRLocalArray::IRLocalArrayValue to represent the value at the offset within the array </returns>
        IRLocalArrayValue operator[](llvm::Value* offset) const;

        /// <summary> Indexing operator to return a reference to the specified offset </summary>
        ///
        /// <param name="offset"> The offset where the value lies within the wrapped array. </param>
        /// <return> An instance of IRLocalArray::IRLocalArrayValue to represent the value at the offset within the array </returns>
        IRLocalArrayValue operator[](int offset) const;
    };
}
}
