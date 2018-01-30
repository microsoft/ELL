////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMUtilities.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LLVMUtilities.h"
#include "EmitterException.h"

// llvm
#include <llvm/IR/Type.h>

namespace ell
{
namespace emitters
{
    //
    // Type conversion from emitter types to LLVM types
    //
    // LLVMTypeList GetLLVMTypes(const VariableTypeList& types);
    // LLVMTypeList GetLLVMTypes(const VariableTypeList& types)
    // {
    //     LLVMTypeList llvmTypes;
    //     for (auto t : types)
    //     {
    //         llvmTypes.push_back(Type(t));
    //     }

    //     return llvmTypes;
    // }

    // std::vector<NamedLLVMType> GetLLVMTypes(const NamedVariableTypeList& types);

    //
    // Get types from LLVM values
    //
    LLVMTypeList GetLLVMTypes(const std::vector<llvm::Value*> values)
    {
        LLVMTypeList result;
        result.reserve(values.size());
        for (auto v : values)
        {
            result.push_back(v->getType());
        }
        return result;
    }

    emitters::TypedOperator GetOperator(LLVMType type, BinaryOperationType operation)
    {
        if (type->isIntegerTy())
        {
            return GetIntegerOperator(operation);
        }
        else if(type->isFloatingPointTy())
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
        else if(type->isFloatingPointTy())
        {
            return GetFloatComparison(comparison);
        }

        throw EmitterException(EmitterError::valueTypeNotSupported);
    }

    // TODO:
    // template <typename... ArgTypes>
    // std::vector<LLVMType> GetLLVMTypes(ArgTypes... args);
}
}