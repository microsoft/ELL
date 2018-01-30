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
    IRLocalScalar Sigmoid(IRLocalScalar a);
}
}

#include "../tcc/IRLocalValueOperations.tcc"
