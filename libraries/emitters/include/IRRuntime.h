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
#include "LLVMUtilities.h"

// utilities
#include "Exception.h"

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
        LLVMValue GetCurrentTime(IRFunctionEmitter& function);

        //
        // Standard math functions
        //

        /// <summary> Get the sqrt function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetSqrtFunction();

        /// <summary> Get the abs function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetAbsFunction();

        /// <summary> Get the exp function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetExpFunction();

        /// <summary> Get the log function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetLogFunction();

        /// <summary> Get the sin function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetSinFunction();

        /// <summary> Get the cos function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetCosFunction();

        /// <summary> Get the tanh function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetTanhFunction();

        // emitter types
        LLVMFunction GetSqrtFunction(VariableType argType);
        LLVMFunction GetAbsFunction(VariableType argType);
        LLVMFunction GetExpFunction(VariableType argType);
        LLVMFunction GetLogFunction(VariableType argType);
        LLVMFunction GetTanhFunction(VariableType argType);
        LLVMFunction GetSinFunction(VariableType argType);
        LLVMFunction GetCosFunction(VariableType argType);

        // llvm types
        LLVMFunction GetSqrtFunction(LLVMType argType);
        LLVMFunction GetAbsFunction(LLVMType argType);
        LLVMFunction GetExpFunction(LLVMType argType);
        LLVMFunction GetLogFunction(LLVMType argType);
        LLVMFunction GetSinFunction(LLVMType argType);
        LLVMFunction GetCosFunction(LLVMType argType);

        //
        // Dot product
        //

        /// <summary> Get the dot product function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetDotProductFunction();

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
        LLVMFunction GetGEMVFunction(bool useBlas);

        /// <summary> Get the BLAS gemm function for the given type. </summary>
        ///
        /// <typeparam name="ValueType"> The data type used (`float` or `double`) </typeparam>
        /// <param name="useBlas"> Indicates whether or not to use BLAS to perform the operation. <param>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetGEMMFunction(bool useBlas);

        // Special OpenBLAS utility functions

        /// <summary> Get the OpenBLAS function for getting the number of threads </summary>
        LLVMFunction GetOpenBLASGetNumThreadsFunction();

        /// <summary> Get the OpenBLAS function for setting the number of threads </summary>
        LLVMFunction GetOpenBLASSetNumThreadsFunction();

        /// <summary> Get the string compare function </summary>
        LLVMFunction GetStringCompareFunction();

        //
        // Posix functions
        //
        IRPosixRuntime& GetPosixEmitter() { return _posixRuntime; }

    private:
        friend IRModuleEmitter;
        IRRuntime(IRModuleEmitter& module);

        LLVMType GetIntType(); // returns LLVM type for native `int`

        std::string GetNamespacePrefix() const;

        //
        // Getting pointers to functions
        //

        // time
        LLVMFunction GetCurrentTimeFunction(); // returns a double containing the current time (in _milliseconds_ from some arbitrary start time)
        LLVMFunction ResolveCurrentTimeFunction(llvm::StructType* timespecType);

        // math
        LLVMFunction GetDotProductIntFunction();
        LLVMFunction GetDotProductFloatFunction();

        // Matrix math (BLAS or native)
        LLVMFunction GetSGEMVFunction(bool useBlas);
        LLVMFunction GetSGEMMFunction(bool useBlas);
        LLVMFunction GetDGEMVFunction(bool useBlas);
        LLVMFunction GetDGEMMFunction(bool useBlas);

        IRModuleEmitter& _module;

        // Special runtimes for different environments or libraries
        IRPosixRuntime _posixRuntime;

        LLVMFunction _dotProductFunctionFloat = nullptr;
        LLVMFunction _dotProductFunction = nullptr;
        LLVMFunction _getCurrentTimeFunction = nullptr;
        LLVMFunction _stringCompareFunction = nullptr;
    };
} // namespace emitters
} // namespace ell

#include "../tcc/IRRuntime.tcc"
