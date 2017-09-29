////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRRuntime.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "LLVMInclude.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace emitters
{
    class IRModuleEmitter;

    /// <summary> Manages external as well as compiler auto-generated functions </summary>
    class IRRuntime
    {
    public:
        /// <summary> Construct a new runtime </summary>
        ///
        /// <param name="module"> The module being compiled. </param>
        IRRuntime(IRModuleEmitter& module);

        //
        // Misc runtime functions
        //

        /// <summary> Get a pointer to the device-side time function. </summary>
        ///
        /// <returns> An LLVM function pointer to the current time function. </returns>
        llvm::Function* GetCurrentTimeFunction(); // returns a double containing the current time (in _milliseconds_ from some arbitrary start time)

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
        /// <typeparam name="ValueType"> The data type used (`float` or `double`). <typeparam>
        /// <param name="useBlas"> Indicates whether or not to use BLAS to perform the operation. <param>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetGEMVFunction(bool useBlas);

        /// <summary> Get the BLAS gemm function for the given type. </summary>
        ///
        /// <typeparam name="ValueType"> The data type used (`float` or `double`) <typeparam>
        /// <param name="useBlas"> Indicates whether or not to use BLAS to perform the operation. <param>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        llvm::Function* GetGEMMFunction(bool useBlas);

    private:
        std::string GetNamespacePrefix() const;

        llvm::Function* GetSqrtFunction(VariableType argType);
        llvm::Function* GetAbsFunction(VariableType argType);
        llvm::Function* GetExpFunction(VariableType argType);
        llvm::Function* GetLogFunction(VariableType argType);

        llvm::Function* GetDotProductIntFunction();
        llvm::Function* GetDotProductFloatFunction();

        llvm::Function* GetSGEMVFunction(bool useBlas);
        llvm::Function* GetSGEMMFunction(bool useBlas);
        llvm::Function* GetDGEMVFunction(bool useBlas);
        llvm::Function* GetDGEMMFunction(bool useBlas);

        llvm::Function* ResolveCurrentTimeFunction(llvm::StructType* timespecType);

        IRModuleEmitter& _module;

        llvm::Function* _pDotProductFunctionFloat = nullptr;
        llvm::Function* _pDotProductFunction = nullptr;
        llvm::Function* _pGetCurrentTimeFunction = nullptr;
    };
}
}

#include "../tcc/IRRuntime.tcc"
