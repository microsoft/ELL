////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableIRFunction.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "IRFunctionEmitter.h"

// llvm
#include <llvm/IR/Value.h>

// stl
#include <string>

namespace ell
{
namespace emitters
{
    /// <summary> Superclass for functions that can emit themselves as LLVM IR </summary>
    template <typename ReturnType, typename... ArgTypes>
    class CompilableIRFunction
    {
    public:
        template <typename ArgType>
        using Value = llvm::Value*;

        /// <summary> Computes the return value of the function </summary>
        ///
        /// <param name="args"> The arguments for the function </param>
        /// <returns> The result of applying the function to the input arguments </returns>
        virtual ReturnType Compute(ArgTypes... args) const = 0;

        /// <summary> Emits LLVM IR that computes the function </summary>
        ///
        /// <param name="function"> The function currently being emitted </param>
        /// <param name="args"> The arguments for the function </param>
        /// <returns> The result of applying the function to the input arguments </returns>
        virtual llvm::Value* Compile(IRFunctionEmitter& function, Value<ArgTypes>... args) const = 0;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const = 0;
    };

    /// <summary> A compilable function that adds two values </summary>
    template <typename ValueType>
    class IRAddFunction : public CompilableIRFunction<ValueType, ValueType, ValueType>
    {
    public:
        /// <summary> Computes the sum of the input arguments </summary>
        ///
        /// <param name="x"> One of the values to add </param>
        /// <param name="y"> The other value to add </param>
        /// <returns> The sum x+y </returns>
        ValueType Compute(ValueType x, ValueType y) const override;

        /// <summary> Emits LLVM IR that computes the sum of two values </summary>
        ///
        /// <param name="x"> One of the values to add </param>
        /// <param name="y"> The other value to add </param>
        /// <returns> The sum x+y </returns>
        llvm::Value* Compile(IRFunctionEmitter& function, llvm::Value* x, llvm::Value* y) const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "add"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }
    };
}
}

#include "../tcc/CompilableIRFunction.tcc"
