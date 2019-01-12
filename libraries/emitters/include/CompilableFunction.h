////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableFunction.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "IRFunctionEmitter.h"
#include "IRLocalScalar.h"
#include "LLVMUtilities.h"

#include <string>
#include <type_traits>

namespace ell
{
namespace emitters
{
    template <typename T>
    class CompilableFunction; // undefined

    /// <summary> Superclass for functions that can emit themselves as LLVM IR </summary>
    template <typename ReturnType, typename... ArgTypes>
    class CompilableFunction<ReturnType(ArgTypes...)>
    {
        static_assert(std::is_fundamental_v<ReturnType>, "ReturnType must be a fundamenal type");
        static_assert(std::conjunction_v<std::is_fundamental<ArgTypes>...>, "ArgTypes must be fundamenal types");

    public:
        CompilableFunction() = default;
        CompilableFunction(CompilableFunction&&) = default;
        CompilableFunction& operator=(CompilableFunction&&) = default;
        virtual ~CompilableFunction() = default;

        template <typename ArgType>
        using Value = IRLocalScalar;

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
        virtual IRLocalScalar Compile(IRFunctionEmitter& function, Value<ArgTypes>... args) const = 0;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const = 0;

    private:
        CompilableFunction(const CompilableFunction&) = delete;
        CompilableFunction& operator=(const CompilableFunction&) = delete;
    };

    template <typename ValueType>
    class IRAddFunction;
} // namespace emitters
} // namespace ell

#pragma region implementation

namespace ell
{
namespace emitters
{
    template <typename ValueType>
    class IRAddFunction : public CompilableFunction<ValueType(ValueType, ValueType)>
    {
    public:
        ValueType Compute(ValueType x, ValueType y) const override { return x + y; }
        IRLocalScalar Compile(IRFunctionEmitter& function, IRLocalScalar x, IRLocalScalar y) const override { return x + y; };
        static std::string GetTypeName() { return "add"; }
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }
    };
} // namespace emitters
} // namespace ell

#pragma endregion implementation
