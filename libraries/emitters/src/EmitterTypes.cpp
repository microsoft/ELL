////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterTypes.cpp (emitter)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EmitterTypes.h"
#include "EmitterException.h"

namespace ell
{
namespace emitters
{
    template <>
    TypedOperator GetOperator<double>(BinaryOperationType operation)
    {
        switch (operation)
        {
            case BinaryOperationType::add:
                return TypedOperator::addFloat;
            case BinaryOperationType::subtract:
                return TypedOperator::subtractFloat;
            case BinaryOperationType::coordinatewiseMultiply:
                return TypedOperator::multiplyFloat;
            case BinaryOperationType::coordinatewiseDivide:
                return TypedOperator::divideFloat;
            default:
                throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
        }
    }

    template <>
    TypedOperator GetOperator<int>(BinaryOperationType operation)
    {
        switch (operation)
        {
            case BinaryOperationType::add:
                return TypedOperator::add;
            case BinaryOperationType::subtract:
                return TypedOperator::subtract;
            case BinaryOperationType::coordinatewiseMultiply:
                return TypedOperator::multiply;
            case BinaryOperationType::coordinatewiseDivide:
                return TypedOperator::divideSigned;
            default:
                throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
        }
    }

    template <>
    TypedOperator GetOperator<bool>(BinaryOperationType operation)
    {
        switch (operation)
        {
            case BinaryOperationType::logicalAnd:
                return TypedOperator::logicalAnd;
            case BinaryOperationType::logicalOr:
                return TypedOperator::logicalOr;
            case BinaryOperationType::logicalXor:
                return TypedOperator::logicalXor;
            default:
                throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
        }
    }

    template <>
    TypedComparison GetComparison<double>(BinaryPredicateType predicate)
    {
        switch (predicate)
        {
            case BinaryPredicateType::equal:
                return TypedComparison::equalsFloat;
            case BinaryPredicateType::notEqual:
                return TypedComparison::notEqualsFloat;
            case BinaryPredicateType::greater:
                return TypedComparison::greaterThanFloat;
            case BinaryPredicateType::greaterOrEqual:
                return TypedComparison::greaterThanOrEqualsFloat;
            case BinaryPredicateType::less:
                return TypedComparison::lessThanFloat;
            case BinaryPredicateType::lessOrEqual:
                return TypedComparison::lessThanOrEqualsFloat;
            default:
                throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
        }
    }

    template <>
    TypedComparison GetComparison<int>(BinaryPredicateType predicate)
    {
        switch (predicate)
        {
            case BinaryPredicateType::equal:
                return TypedComparison::equals;
            case BinaryPredicateType::notEqual:
                return TypedComparison::notEquals;
            case BinaryPredicateType::greater:
                return TypedComparison::greaterThan;
            case BinaryPredicateType::greaterOrEqual:
                return TypedComparison::greaterThanOrEquals;
            case BinaryPredicateType::less:
                return TypedComparison::lessThan;
            case BinaryPredicateType::lessOrEqual:
                return TypedComparison::lessThanOrEquals;
            default:
                throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
        }
    }
    
    template <>
    VariableType GetVariableType<char>()
    {
        return VariableType::Char8;
    }

    template <>
    VariableType GetVariableType<char*>()
    {
        return VariableType::Char8Pointer;
    }

    template <>
    VariableType GetVariableType<bool>()
    {
        return VariableType::Byte;
    }

    template <>
    VariableType GetVariableType<bool*>()
    {
        return VariableType::BytePointer;
    }

    template <>
    VariableType GetVariableType<uint8_t>()
    {
        return VariableType::Byte;
    }

    template <>
    VariableType GetVariableType<uint8_t*>()
    {
        return VariableType::BytePointer;
    }

    template <>
    VariableType GetVariableType<short>()
    {
        return VariableType::Short;
    }

    template <>
    VariableType GetVariableType<short*>()
    {
        return VariableType::ShortPointer;
    }

    template <>
    VariableType GetVariableType<int>()
    {
        return VariableType::Int32;
    }

    template <>
    VariableType GetVariableType<int*>()
    {
        return VariableType::Int32Pointer;
    }

    template <>
    VariableType GetVariableType<int64_t>()
    {
        return VariableType::Int64;
    }

    template <>
    VariableType GetVariableType<int64_t*>()
    {
        return VariableType::Int64Pointer;
    }

    template <>
    VariableType GetVariableType<double>()
    {
        return VariableType::Double;
    }

    template <>
    VariableType GetVariableType<double*>()
    {
        return VariableType::DoublePointer;
    }

    template <>
    double GetDefaultValue<double>()
    {
        return 0.0;
    }

    template <>
    int GetDefaultValue<int>()
    {
        return 0;
    }

    VariableType GetPointerType(VariableType type)
    {
        switch (type)
        {
            case VariableType::Void:
                return VariableType::VoidPointer;
            case VariableType::Byte:
                return VariableType::BytePointer;
            case VariableType::Short:
                return VariableType::ShortPointer;
            case VariableType::Int32:
                return VariableType::Int32Pointer;
            case VariableType::Int64:
                return VariableType::Int64Pointer;
            case VariableType::Double:
                return VariableType::DoublePointer;
            case VariableType::Char8:
                return VariableType::Char8Pointer;
            default:
                break;
        }
        return type;
    }

    template <>
    TypedOperator GetAddForValueType<double>()
    {
        return TypedOperator::addFloat;
    }

    template <>
    TypedOperator GetAddForValueType<int>()
    {
        return TypedOperator::add;
    }

    template <>
    TypedOperator GetSubtractForValueType<double>()
    {
        return TypedOperator::subtractFloat;
    }

    template <>
    TypedOperator GetSubtractForValueType<int>()
    {
        return TypedOperator::subtract;
    }

    template <>
    TypedOperator GetMultiplyForValueType<double>()
    {
        return TypedOperator::multiplyFloat;
    }

    template <>
    TypedOperator GetMultiplyForValueType<int>()
    {
        return TypedOperator::multiply;
    }

    template <>
    TypedOperator GetDivideForValueType<double>()
    {
        return TypedOperator::divideFloat;
    }

    template <>
    TypedOperator GetDivideForValueType<int>()
    {
        return TypedOperator::divideSigned;
    }

    bool IsSigned(VariableType type)
    {
        switch (type)
        {
            case VariableType::Short:
            case VariableType::Int32:
            case VariableType::Int64:
            case VariableType::Double:
                return true;

            default:
                return false;
        }
    }
}
}