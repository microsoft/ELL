////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMUtilities.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LLVMUtilities.h"
#include "EmitterException.h"

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>

namespace ell
{
namespace emitters
{
    //
    // Get types from LLVM values
    //
    LLVMTypeList GetLLVMTypes(const std::vector<LLVMValue>& values)
    {
        LLVMTypeList result;
        result.reserve(values.size());
        for (auto v : values)
        {
            result.push_back(v->getType());
        }
        return result;
    }

    emitters::TypedOperator GetOperator(LLVMType type, BinaryOperatorType operation)
    {
        if (type->isIntegerTy() && type->getIntegerBitWidth() == 1)
        {
            return GetBooleanOperator(operation);
        }
        else if (type->isIntegerTy())
        {
            return GetIntegerOperator(operation);
        }
        else if (type->isFloatingPointTy())
        {
            return GetFloatOperator(operation);
        }

        throw EmitterException(EmitterError::valueTypeNotSupported);
    }

    emitters::TypedComparison GetComparison(LLVMType type, BinaryPredicateType comparison)
    {
        if (type->isIntegerTy())
        {
            return GetIntegerComparison(comparison);
        }
        else if (type->isFloatingPointTy())
        {
            return GetFloatComparison(comparison);
        }

        throw EmitterException(EmitterError::valueTypeNotSupported);
    }

    VariableType ToVariableType(LLVMType type)
    {
        if (type->isPointerTy())
        {
            auto et = type->getPointerElementType();
            switch (ToVariableType(et))
            {
            case VariableType::Void:
                return VariableType::VoidPointer;
            case VariableType::Double:
                return VariableType::DoublePointer;
            case VariableType::Float:
                return VariableType::FloatPointer;
            case VariableType::Byte:
                return VariableType::BytePointer;
            case VariableType::Int16:
                return VariableType::Int16Pointer;
            case VariableType::Int32:
                return VariableType::Int32Pointer;
            case VariableType::Int64:
                return VariableType::Int64Pointer;
            default:
                break;
            }
        }
        else
        {
            if (type->isVoidTy()) {
                return VariableType::Void;
            }
            else if (type->isDoubleTy()) {
                return VariableType::Double;
            }
            else if (type->isFloatTy()) {
                return VariableType::Float;
            }
            else if (type->isIntegerTy()) {
                switch (type->getIntegerBitWidth())
                {
                case 8:
                    return VariableType::Byte;
                case 16:
                    return VariableType::Int16;
                case 32:
                    return VariableType::Int32;
                case 64:
                    return VariableType::Int64;
                }
            }
        }
        return VariableType::Custom;
    }

} // namespace emitters
} // namespace ell
