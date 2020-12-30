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

#include <utilities/include/Exception.h>

#include <llvm/IR/Value.h>

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
    } // namespace detail
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
        auto op = GetOperator(type, emitters::BinaryOperatorType::add);

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator-=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);

        auto type = this->value->getType();
        auto op = GetOperator(type, emitters::BinaryOperatorType::subtract);

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator*=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);

        auto type = this->value->getType();
        auto op = GetOperator(type, emitters::BinaryOperatorType::multiply);

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator/=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);

        auto type = this->value->getType();
        auto op = GetOperator(type, emitters::BinaryOperatorType::divide);

        value = function.Operator(op, value, rhs);

        return *this;
    }

    IRLocalScalar& IRLocalScalar::operator%=(const IRLocalScalar& rhs)
    {
        VerifyArgTypesCompatible(*this, rhs);

        auto type = this->value->getType();
        auto op = GetOperator(type, emitters::BinaryOperatorType::modulo);

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

    IRLocalScalar operator+(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) + b;
    }

    IRLocalScalar operator+(IRLocalScalar a, LLVMValue b)
    {
        return a + IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator-(IRLocalScalar a, IRLocalScalar b)
    {
        a -= b;
        return a;
    }

    IRLocalScalar operator-(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) - b;
    }

    IRLocalScalar operator-(IRLocalScalar a, LLVMValue b)
    {
        return a - IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator-(IRLocalScalar a)
    {
        auto type = a.value->getType();
        auto zero = a.function.GetEmitter().Zero(type);
        auto op = GetOperator(type, emitters::BinaryOperatorType::subtract);
        return { a.function, a.function.Operator(op, zero, a) };
    }

    IRLocalScalar operator*(IRLocalScalar a, IRLocalScalar b)
    {
        a *= b;
        return a;
    }

    IRLocalScalar operator*(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) * b;
    }

    IRLocalScalar operator*(IRLocalScalar a, LLVMValue b)
    {
        return a * IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator/(IRLocalScalar a, IRLocalScalar b)
    {
        a /= b;
        return a;
    }

    IRLocalScalar operator/(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) / b;
    }

    IRLocalScalar operator/(IRLocalScalar a, LLVMValue b)
    {
        return a / IRLocalScalar(a.function, b);
    }

    IRLocalScalar operator%(IRLocalScalar a, IRLocalScalar b)
    {
        a %= b;
        return a;
    }

    IRLocalScalar operator%(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) % b;
    }

    IRLocalScalar operator%(IRLocalScalar a, LLVMValue b)
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

    IRLocalScalar operator==(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) == b;
    }

    IRLocalScalar operator==(IRLocalScalar a, LLVMValue b)
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

    IRLocalScalar operator!=(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) != b;
    }

    IRLocalScalar operator!=(IRLocalScalar a, LLVMValue b)
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

    IRLocalScalar operator<(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) < b;
    }

    IRLocalScalar operator<(IRLocalScalar a, LLVMValue b)
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

    IRLocalScalar operator<=(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) <= b;
    }

    IRLocalScalar operator<=(IRLocalScalar a, LLVMValue b)
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

    IRLocalScalar operator>(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) > b;
    }

    IRLocalScalar operator>(IRLocalScalar a, LLVMValue b)
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

    IRLocalScalar operator>=(LLVMValue a, IRLocalScalar b)
    {
        return IRLocalScalar(b.function, a) >= b;
    }

    IRLocalScalar operator>=(IRLocalScalar a, LLVMValue b)
    {
        return a >= IRLocalScalar(a.function, b);
    }

} // namespace emitters
} // namespace ell
