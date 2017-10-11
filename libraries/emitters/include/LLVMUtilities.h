////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMUtilities.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterTypes.h"

// llvm
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

namespace ell
{
namespace emitters
{
    /// <summary> Nice name for llvm::Type pointers </summary>
    using LLVMType = llvm::Type*;

    /// <summary> An LLVM type with a name (e.g., for arguments and struct fields) </summary>
    using NamedLLVMType = std::pair<std::string, LLVMType>;

    /// <summary> Collections of LLVM types </summary>
    using LLVMTypeList = std::vector<LLVMType>;

    /// <summary> Collections of named LLVM types </summary>
    using NamedLLVMTypeList = std::vector<NamedLLVMType>;

    //
    // Type conversion from emitter types to LLVM types
    //
    std::vector<LLVMType> GetLLVMTypes(const VariableTypeList& types);
    std::vector<NamedLLVMType> GetLLVMTypes(const NamedVariableTypeList& types);
    
    //
    // Get types from LLVM values
    //
    std::vector<LLVMType> GetLLVMTypes(const std::vector<llvm::Value*> values);

    // TODO:
    // template <typename... ArgTypes>
    // std::vector<LLVMType> GetLLVMTypes(ArgTypes... args);
}
}