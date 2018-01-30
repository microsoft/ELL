////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRRuntime.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "IRPosixRuntime.h"

// utilities
#include "Exception.h"

// llvm
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace ell
{
namespace emitters
{
    class IRModuleEmitter;
    class IRFunctionEmitter;

    /// <summary> Manages external as well as compiler auto-generated functions </summary>
    class IRRuntime
    {
    public:
        //
        // Misc runtime functions
        //
        llvm::Value* GetCurrentTime(IRFunctionEmitter& function);

        //
        // Standard math functions
        //

        /// <summary> Get the sqrt function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetSqrtFunction();

        /// <summary> Get the abs function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetAbsFunction();

        /// <summary> Get the exp function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetExpFunction();

        /// <summary> Get the log function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetLogFunction();

        /// <summary> Get the sin function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetSinFunction();

        /// <summary> Get the cos function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetCosFunction();

        /// <summary> Get the tanh function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetTanhFunction();

        // emitter types
        llvm::Function* GetSqrtFunction(VariableType argType);
        llvm::Function* GetAbsFunction(VariableType argType);
        llvm::Function* GetExpFunction(VariableType argType);
        llvm::Function* GetLogFunction(VariableType argType);
        llvm::Function* GetTanhFunction(VariableType argType);
        llvm::Function* GetSinFunction(VariableType argType);
        llvm::Function* GetCosFunction(VariableType argType);

        // llvm types
        llvm::Function* GetSqrtFunction(llvm::Type* argType);
        llvm::Function* GetAbsFunction(llvm::Type* argType);
        llvm::Function* GetExpFunction(llvm::Type* argType);
        llvm::Function* GetLogFunction(llvm::Type* argType);
        llvm::Function* GetSinFunction(llvm::Type* argType);
        llvm::Function* GetCosFunction(llvm::Type* argType);

        //
        // Dot product
        //

        /// <summary> Get the dot product function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetDotProductFunction();

        //
        // Matrix functions
        //

        /// <summary> Get the BLAS gemv function for the given type. </summary>
        ///
        /// <typeparam name="ValueType"> The data type used (`float` or `double`). </typeparam>
        /// <param name="useBlas"> Indicates whether or not to use BLAS to perform the operation. <param>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetGEMVFunction(bool useBlas);

        /// <summary> Get the BLAS gemm function for the given type. </summary>
        ///
        /// <typeparam name="ValueType"> The data type used (`float` or `double`) </typeparam>
        /// <param name="useBlas"> Indicates whether or not to use BLAS to perform the operation. <param>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetGEMMFunction(bool useBlas);

        // Special OpenBLAS utility functions

        /// <summary> Get the OpenBLAS function for getting the number of threads </summary>
        llvm::Function* GetOpenBLASGetNumThreadsFunction();

        /// <summary> Get the OpenBLAS function for setting the number of threads </summary>
        llvm::Function* GetOpenBLASSetNumThreadsFunction();

        //
        // Posix functions
        //
        IRPosixRuntime& GetPosixEmitter() { return _posixRuntime; }

    private:
        friend IRModuleEmitter;
        IRRuntime(IRModuleEmitter& module);

        llvm::Type* GetIntType(); // returns LLVM type for native `int`

        std::string GetNamespacePrefix() const;

        //
        // Getting pointers to functions
        //

        // time
        llvm::Function* GetCurrentTimeFunction(); // returns a double containing the current time (in _milliseconds_ from some arbitrary start time)
        llvm::Function* ResolveCurrentTimeFunction(llvm::StructType* timespecType);

        // math
        llvm::Function* GetDotProductIntFunction();
        llvm::Function* GetDotProductFloatFunction();

        // Matrix math (BLAS or native)
        llvm::Function* GetSGEMVFunction(bool useBlas);
        llvm::Function* GetSGEMMFunction(bool useBlas);
        llvm::Function* GetDGEMVFunction(bool useBlas);
        llvm::Function* GetDGEMMFunction(bool useBlas);

        IRModuleEmitter& _module;

        // Special runtimes for different environments or libraries
        IRPosixRuntime _posixRuntime;

        llvm::Function* _pDotProductFunctionFloat = nullptr;
        llvm::Function* _pDotProductFunction = nullptr;
        llvm::Function* _pGetCurrentTimeFunction = nullptr;
    };
}
}

#include "../tcc/IRRuntime.tcc"
