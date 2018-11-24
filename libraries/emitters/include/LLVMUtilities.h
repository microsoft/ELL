////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LLVMUtilities.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterTypes.h"

namespace llvm
{
class Function;
class FunctionType;
class GlobalVariable;
class StructType;
class Type;
class Value;
} // namespace llvm

namespace ell
{
namespace emitters
{
    /// <summary> Nice name for llvm::Function pointers. </summary>
    using LLVMFunction = llvm::Function*;

    /// <summary> Nice name for llvm::Type pointers. </summary>
    using LLVMType = llvm::Type*;

    /// <summary> Nice name for llvm::Value pointers. </summary>
    using LLVMValue = llvm::Value*;

    /// <summary> An LLVM type with a name (e.g., for arguments and struct fields). </summary>
    using NamedLLVMType = std::pair<std::string, LLVMType>;

    /// <summary> Collections of LLVM types. </summary>
    using LLVMTypeList = std::vector<LLVMType>;

    /// <summary> Collections of named LLVM types. </summary>
    using NamedLLVMTypeList = std::vector<NamedLLVMType>;

    //
    // Get types from LLVM values
    //

    /// <summary> Get the LLVM types from a vector of LLVM values. </summary>
    ///
    /// <param name="values"> The values to get the types of. </param>
    ///
    /// <returns> The types for the given values. </returns>
    LLVMTypeList GetLLVMTypes(const std::vector<LLVMValue>& values);

    /// <summary> Get the typed version of a BinaryOperationType for a given LLVM type. </summary>
    ///
    /// <param name="type"> The LLVM type of the values to operate on. </param>
    /// <param name="operation"> The (untyped) BinaryOperationType of the operation. </param>
    ///
    /// <returns> The TypedOperator for operating on values of the given type. </returns>
    emitters::TypedOperator GetOperator(LLVMType type, BinaryOperationType operation);

    /// <summary> Get the typed version of a BinaryPredicateType for a given LLVM type. </summary>
    ///
    /// <param name="type"> The LLVM type of the values to compare. </param>
    /// <param name="operation"> The (untyped) BinaryPredicateType of the comparison. </param>
    ///
    /// <returns> The TypedComparison for comparing values of the given type. </returns>
    emitters::TypedComparison GetComparison(LLVMType type, BinaryPredicateType operation);
} // namespace emitters
} // namespace ell
