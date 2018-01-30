////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRMetadata.h (emitters)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// llvm
#include <llvm/IR/Function.h>

// stl
#include <string>
#include <unordered_set>
#include <vector>

namespace ell
{
namespace emitters
{
    class IRModuleEmitter;

    //
    // Metadata tags
    //

    /// <summary> Indicates that a function is a callback that should be wrapped by SWIG. </summary>
    static const std::string c_callbackFunctionTagName = "ell.fn.callback";

    /// <summary> Indicates that a function should be declared in the generated C/C++ header. </summary>
    /// <remarks>
    /// Set a function-level tag with an empty value.
    /// </remarks>
    static const std::string c_declareFunctionInHeaderTagName = "ell.header.declareFn";

    /// <summary> Indicates that a type should be declared in the generated C/C++ header. </summary>
    /// <remarks>
    /// Set a module-level tag, using the type name as the value.
    /// </remarks>
    static const std::string c_declareTypeInHeaderTagName = "ell.header.declareType";

    /// <summary> Indicates the Predict function that should be wrapped by SWIG. </summary>
    static const std::string c_predictFunctionTagName = "ell.fn.predict";

    /// <summary> Indicates a function that should be wrapped by SWIG. </summary>
    static const std::string c_swigFunctionTagName = "ell.fn.swig";

    /// <summary> Indicates the Step function that should be wrapped by SWIG. </summary>
    /// <remarks>
    /// Set the value to the output element count.
    /// </remarks>
    static const std::string c_stepFunctionTagName = "ell.fn.step";

    /// <summary> Indicates the time functions associated with Step, such as GetInterval(), that should be wrapped by SWIG. </summary>
    /// <remarks>
    /// Set the value to the API name of the function.
    /// </remarks>
    static const std::string c_stepTimeFunctionTagName = "ell.fn.stepTime";

    /// <summary> Gets tag to Indicate the names of a struct's fields. </summary>
    /// <remarks>
    /// Returns a module-level tag, with the type name encoded in the name and field names as the value.
    /// </remarks>
    std::string GetStructFieldsTagName(llvm::StructType* structType);

    //
    // Utilities for reading metadata (that wrap IRModuleEmitter)
    //

    /// <summary> Holds a pointer to an llvm::Function and a set of tag values. </summary>
    struct FunctionTagValues
    {
        llvm::Function* function;
        std::vector<std::string> values;
    };

    /// <summary> Gets functions associated with a function-level metadata tag. </summary>
    ///
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to search </param>
    /// <param name="tag"> The function-level metadata tag. </param>
    ///
    /// <returns> A vector of LLVM functions with values for the given metadata tag. </returns>
    std::vector<FunctionTagValues> GetFunctionsWithTag(IRModuleEmitter& moduleEmitter, const std::string& tag);

    /// <summary> Gets singleton values associated with a module-level metadata tag. </summary>
    ///
    /// <param name="moduleEmitter"> The `IRModuleEmitter` containing the module to search </param>
    /// <param name="tag"> The global metadata tag. </param>
    ///
    /// <returns> An unordered_set of values for the given metadata tag. </returns>
    std::unordered_set<std::string> GetSingletonModuleTagValues(IRModuleEmitter& moduleEmitter, const std::string& tag);
}
}
