////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRLocalScalar.cpp (emitter)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRLocalScalar.h"
#include "EmitterException.h"
#include "IRModuleEmitter.h"

// utilities
#include "Exception.h"

// LLVM
#include <llvm/IR/Value.h>

// stl
#include <functional>

namespace ell
{
namespace emitters
{
    namespace detail
    {
        // This function is needed to break the cyclical dependency between
        // IRFunctionEmitter and IRLocalScalar
        IREmitter& GetEmitter(IRFunctionEmitter& function)
        {
            return function.GetEmitter();
        }
    }
    using namespace detail;

    bool IRLocalScalar::IsConstantInt() const
    {
        return llvm::isa<llvm::ConstantInt>(this->value);
    }

    bool IRLocalScalar::IsConstantFloat() const
    {
        return llvm::isa<llvm::ConstantFP>(this->value);
    }

    template <>
    float IRLocalScalar::GetFloatValue<float>() const
    {
        auto floatValue = llvm::cast<llvm::ConstantFP>(this->value);
        return floatValue->getValueAPF().convertToFloat();
    }

    template <>
    double IRLocalScalar::GetFloatValue<double>() const
    {
        auto floatValue = llvm::cast<llvm::ConstantFP>(this->value);
        return floatValue->getValueAPF().convertToDouble();
    }

    IRLocalScalar& IRLocalScalar::operator+=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);

        auto type = this->value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::add);

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator-=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);

        auto type = this->value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::subtract);

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator*=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);

        auto type = this->value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::coordinatewiseMultiply);

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator/=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);

        auto type = this->value->getType();
        auto op = GetOperator(type, emitters::BinaryOperationType::coordinatewiseDivide);

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator%=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs, BothIntegral);

        const auto op = emitters::TypedOperator::moduloSigned;

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator|=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);
        const auto op = emitters::TypedOperator::logicalOr;

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator&=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);
        const auto op = emitters::TypedOperator::logicalAnd;
        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator^=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);
        const auto op = emitters::TypedOperator::logicalXor;
        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator<<=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);
        const auto op = emitters::TypedOperator::shiftLeft;
        value = function.Operator(op, value, rhs);

        return *this;
    }

    //
    // Arithmetic operators
    //
    IRLocalScalar operator+(IRLocalScalar a, IRLocalScalar b)
    {
        a += b;
        return a;
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
        a -= b;
        return a;
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
        a *= b;
        return a;
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
        a /= b;
        return a;
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
        a %= b;
        return a;
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
        a &= b;
        return a;
    }

    IRLocalScalar operator|(IRLocalScalar a, IRLocalScalar b)
    {
        a |= b;
        return a;
    }

    IRLocalScalar operator^(IRLocalScalar a, IRLocalScalar b)
    {
        a ^= b;
        return a;
    }

    IRLocalScalar operator<<(IRLocalScalar a, IRLocalScalar b)
    {
        a <<= b;
        return a;
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

    IRLocalScalar Min(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        llvm::Value* result = a.function.Select(a < b, a, b);
        return { a.function, result };
    }

    IRLocalScalar Max(IRLocalScalar a, IRLocalScalar b)
    {
        VerifyArgTypesCompatible(a, b);
        llvm::Value* result = a.function.Select(a >= b, a, b);
        return { a.function, result };
    }
}
}
