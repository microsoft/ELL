////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMUtilities.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LLVMUtilities.h"

// llvm
#include "llvm/IR/Type.h"

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
        for(auto v: values)
        {
            result.push_back(v->getType());
        }
        return result;
    }

    // TODO:
    // template <typename... ArgTypes>
    // std::vector<LLVMType> GetLLVMTypes(ArgTypes... args);
}
}