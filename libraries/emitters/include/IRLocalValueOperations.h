////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValueOperations.h (emitters)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRFunctionEmitter.h"
#include "IRLocalValue.h"

namespace ell
{
namespace emitters
{
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
}
}

#include "../tcc/IRLocalValueOperations.tcc"
