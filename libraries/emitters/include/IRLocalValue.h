////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValue.h (emitters)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <functional>

namespace llvm
{
    class Value;
}

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;
    struct IRLocalValue;

    namespace detail
    {
        bool BothIntegral(const IRLocalValue& a, const IRLocalValue& b);

        bool BothFloatingPoint(const IRLocalValue& a, const IRLocalValue& b);

        void VerifyArgTypesCompatible(const IRLocalValue& a, const IRLocalValue& b);

        void VerifyArgTypesCompatible(const IRLocalValue& a, const IRLocalValue& b, std::function<bool(const IRLocalValue&, const IRLocalValue&)> areCompatible);
    }

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

        /// <summary> Checks if this value has been assigned to. </summary>
        ///
        /// <returns> Returns `true` if there is a value assigned to this. </returns>
        bool IsValid() const { return value != nullptr; }

        /// <summary> The function this value is in scope for. </summary>
        IRFunctionEmitter& function;

        /// <summary> The llvm::Value* being wrapped. </summary>
        llvm::Value* value;

    private:
        IRLocalValue() = default;
    };

    /// <summary>
    /// Version of IRLocalValue for pointer values
    /// </summary>
    struct IRLocalPointer : public IRLocalValue
    {
        using IRLocalValue::IRLocalValue;
    };
}
}
