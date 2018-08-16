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
    TypedOperator GetFloatOperator(BinaryOperationType operation)
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

    TypedOperator GetIntegerOperator(BinaryOperationType operation)
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

    TypedComparison GetFloatComparison(BinaryPredicateType predicate)
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

    TypedComparison GetIntegerComparison(BinaryPredicateType predicate)
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
    TypedOperator GetOperator<float>(BinaryOperationType operation)
    {
        return GetFloatOperator(operation);
    }

    template <>
    TypedOperator GetOperator<double>(BinaryOperationType operation)
    {
        return GetFloatOperator(operation);
    }

    template <>
    TypedOperator GetOperator<int>(BinaryOperationType operation)
    {
        return GetIntegerOperator(operation);
    }

    template <>
    TypedOperator GetOperator<int64_t>(BinaryOperationType operation)
    {
        return GetIntegerOperator(operation);
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
    TypedComparison GetComparison<float>(BinaryPredicateType predicate)
    {
        return GetFloatComparison(predicate);
    }

    template <>
    TypedComparison GetComparison<double>(BinaryPredicateType predicate)
    {
        return GetFloatComparison(predicate);
    }

    template <>
    TypedComparison GetComparison<int>(BinaryPredicateType predicate)
    {
        return GetIntegerComparison(predicate);
    }

    template <>
    TypedComparison GetComparison<int64_t>(BinaryPredicateType predicate)
    {
        return GetIntegerComparison(predicate);
    }

    template <>
    VariableType GetVariableType<void>()
    {
        return VariableType::Void;
    }

    template <>
    VariableType GetVariableType<void*>()
    {
        return VariableType::VoidPointer;
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
    VariableType GetVariableType<float>()
    {
        return VariableType::Float;
    }

    template <>
    VariableType GetVariableType<float*>()
    {
        return VariableType::FloatPointer;
    }

    template <>
    float GetDefaultValue<float>()
    {
        return 0.0f;
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

    template <>
    int64_t GetDefaultValue<int64_t>()
    {
        return 0L;
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
        case VariableType::Float:
            return VariableType::FloatPointer;
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
    TypedOperator GetAddForValueType<float>()
    {
        return TypedOperator::addFloat;
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
    TypedOperator GetAddForValueType<int64_t>()
    {
        return TypedOperator::add;
    }

    template <>
    TypedOperator GetAddForValueType<bool>()
    {
        throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
    }

    template <>
    TypedOperator GetSubtractForValueType<float>()
    {
        return TypedOperator::subtractFloat;
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
    TypedOperator GetSubtractForValueType<int64_t>()
    {
        return TypedOperator::subtract;
    }

    template <>
    TypedOperator GetSubtractForValueType<bool>()
    {
        throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
    }

    template <>
    TypedOperator GetMultiplyForValueType<float>()
    {
        return TypedOperator::multiplyFloat;
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
    TypedOperator GetMultiplyForValueType<int64_t>()
    {
        return TypedOperator::multiply;
    }

    template <>
    TypedOperator GetMultiplyForValueType<bool>()
    {
        throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
    }

    template <>
    TypedOperator GetDivideForValueType<float>()
    {
        return TypedOperator::divideFloat;
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

    template <>
    TypedOperator GetDivideForValueType<int64_t>()
    {
        return TypedOperator::divideSigned;
    }

    template <>
    TypedOperator GetDivideForValueType<bool>()
    {
        throw EmitterException(EmitterError::binaryOperationTypeNotSupported);
    }

    template <>
    TypedOperator GetModForValueType<int>()
    {
        return TypedOperator::moduloSigned;
    }

    bool IsSigned(VariableType type)
    {
        switch (type)
        {
        case VariableType::Short:
        case VariableType::Int32:
        case VariableType::Int64:
        case VariableType::Float:
        case VariableType::Double:
            return true;

        default:
            return false;
        }
    }
}
}
