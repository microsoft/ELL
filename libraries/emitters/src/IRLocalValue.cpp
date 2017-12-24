////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValue.cpp (emitter)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRLocalValue.h"
#include "EmitterException.h"
#include "IRModuleEmitter.h"

// llvm
#include <llvm/IR/Type.h>

namespace ell
{
namespace emitters
{
    namespace
    {
        void VerifyFromSameFunction(const IRLocalValue& a, const IRLocalValue& b)
        {
            if (a.function.GetFunction() != b.function.GetFunction())
            {
                throw EmitterException(EmitterError::badFunctionArguments, "IRLocalValue arguments are local to different functions");
            }
        }

        bool BothIntegral(const IRLocalValue& a, const IRLocalValue& b)
        {
            return (a.value->getType()->getScalarType()->isIntegerTy() && b.value->getType()->getScalarType()->isIntegerTy());
        }

        bool BothFloatingPoint(const IRLocalValue& a, const IRLocalValue& b)
        {
            return (a.value->getType()->getScalarType()->isFloatingPointTy() && b.value->getType()->getScalarType()->isFloatingPointTy());
        }

        void VerifyArgTypesCompatible(const IRLocalValue& a, const IRLocalValue& b)
        {
            VerifyFromSameFunction(a, b);
            if (!(BothIntegral(a, b) || BothFloatingPoint(a, b)))
            {
                throw EmitterException(EmitterError::badFunctionArguments, "IRLocalValue arguments have incompatible types");
            }
        }
    }

    IRLocalValue::IRLocalValue(emitters::IRFunctionEmitter& function, llvm::Value* value)
        : function(function), value(value) {}

    //
    // Arithmetic operators
    //
    IRLocalScalar operator+(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::add);
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator-(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::subtract);
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator-(IRLocalScalar a)
    {
        auto type = a.value->getType();
        auto zero = a.function.GetEmitter().Zero(type);
        auto op = GetOperator(type, emitters::BinaryOperationType::subtract);
        return { a.function, a.function.Operator(op, zero, a) };
    }

    IRLocalScalar operator*(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::coordinatewiseMultiply);
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator/(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::coordinatewiseDivide);
        return { a.function, a.function.Operator(op, a, b) };
    }

    //
    // Logical operations
    //
    IRLocalScalar operator&&(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        return { a.function, a.function.LogicalAnd(a, b) };
    }

    IRLocalScalar operator||(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        return { a.function, a.function.LogicalOr(a, b) };
    }

    IRLocalScalar operator~(IRLocalScalar a)
    {
        return { a.function, a.function.LogicalNot(a) };
    }

    //
    // Comparison operators
    //
    IRLocalScalar operator==(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::equal);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator!=(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::notEqual);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator<(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::less);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator<=(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::lessOrEqual);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator>(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::greater);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator>=(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::greaterOrEqual);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    //
    // Math functions
    //
    IRLocalScalar Abs(IRLocalScalar a)
    {
        auto f = a.function.GetModule().GetRuntime().GetAbsFunction((a.value)->getType());
        return { a.function, a.function.Call(f, { a }) };
    }

    IRLocalScalar Sqrt(IRLocalScalar a)
    {
        auto f = a.function.GetModule().GetRuntime().GetSqrtFunction((a.value)->getType());
        return { a.function, a.function.Call(f, { a }) };
    }

    IRLocalScalar Exp(IRLocalScalar a)
    {
        auto f = a.function.GetModule().GetRuntime().GetExpFunction((a.value)->getType());
        return { a.function, a.function.Call(f, { a }) };
    }

    IRLocalScalar Log(IRLocalScalar a)
    {
        auto f = a.function.GetModule().GetRuntime().GetLogFunction((a.value)->getType());
        return { a.function, a.function.Call(f, { a }) };
    }

    IRLocalScalar Sin(IRLocalScalar a)
    {
        auto f = a.function.GetModule().GetRuntime().GetSinFunction((a.value)->getType());
        return { a.function, a.function.Call(f, { a }) };
    }

    IRLocalScalar Cos(IRLocalScalar a)
    {
        auto f = a.function.GetModule().GetRuntime().GetCosFunction((a.value)->getType());
        return { a.function, a.function.Call(f, { a }) };
    }
}
}
