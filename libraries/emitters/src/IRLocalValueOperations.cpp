////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalValueOperations.cpp (emitter)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRModuleEmitter.h"
#include "EmitterException.h"
#include "IRLocalValueOperations.h"

// utilities
#include "Exception.h"

// stl
#include <functional>

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
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "IRLocalValue arguments have incompatible types");
            }
        }

        // `areCompatible` is a function that returns true if the arg types are compatible. It has the signature: bool(const IRLocalValue& a, const IRLocalValue& b)
        using CompatibleTypesPredicate = std::function<bool(const IRLocalValue&, const IRLocalValue&)>;
        void VerifyArgTypesCompatible(const IRLocalValue& a, const IRLocalValue& b, CompatibleTypesPredicate areCompatible) 
        {
            VerifyFromSameFunction(a, b);
            if (!(areCompatible(a, b)))
            {
                throw EmitterException(EmitterError::badFunctionArguments, "IRLocalValue arguments have incompatible types");
            }
        }
    }

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

    IRLocalScalar operator+(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) + b;
    }

    IRLocalScalar operator+(IRLocalScalar a, llvm::Value* b)
    {
        return a + IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator-(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::subtract);
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator-(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) - b;
    }

    IRLocalScalar operator-(IRLocalScalar a, llvm::Value* b)
    {
        return a - IRLocalScalar(a.function, b);
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

    IRLocalScalar operator*(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) * b;
    }

    IRLocalScalar operator*(IRLocalScalar a, llvm::Value* b)
    {
        return a * IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator/(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::coordinatewiseDivide);
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator/(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) / b;
    }

    IRLocalScalar operator/(IRLocalScalar a, llvm::Value* b)
    {
        return a / IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator%(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b, BothIntegral);
        const auto op = emitters::TypedOperator::moduloSigned;
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator%(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) % b;
    }

    IRLocalScalar operator%(IRLocalScalar a, llvm::Value* b)
    {
        return a % IRLocalScalar(a.function, b);
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
    // Bitwise operations
    //
    IRLocalScalar operator&(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        const auto op = emitters::TypedOperator::logicalAnd;
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator|(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        const auto op = emitters::TypedOperator::logicalOr;
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator^(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        const auto op = emitters::TypedOperator::logicalXor;
        return { a.function, a.function.Operator(op, a, b) };
    }

    IRLocalScalar operator<<(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        const auto op = emitters::TypedOperator::shiftLeft;
        return { a.function, a.function.Operator(op, a, b) };
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

    IRLocalScalar operator==(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) == b;
    }

    IRLocalScalar operator==(IRLocalScalar a, llvm::Value* b)
    {
        return a == IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator!=(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::notEqual);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator!=(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) != b;
    }

    IRLocalScalar operator!=(IRLocalScalar a, llvm::Value* b)
    {
        return a != IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator<(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::less);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator<(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) < b;
    }

    IRLocalScalar operator<(IRLocalScalar a, llvm::Value* b)
    {
        return a < IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator<=(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::lessOrEqual);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator<=(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) <= b;
    }

    IRLocalScalar operator<=(IRLocalScalar a, llvm::Value* b)
    {
        return a <= IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator>(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::greater);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator>(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) > b;
    }

    IRLocalScalar operator>(IRLocalScalar a, llvm::Value* b)
    {
        return a > IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator>=(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        auto type = a.value->getType();
        auto cmp = GetComparison(type, emitters::BinaryPredicateType::greaterOrEqual);
        return { a.function, a.function.Comparison(cmp, a, b) };
    }

    IRLocalScalar operator>=(llvm::Value* a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) >= b;
    }

    IRLocalScalar operator>=(IRLocalScalar a, llvm::Value* b)
    {
        return a >= IRLocalScalar(a.function, b);
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
