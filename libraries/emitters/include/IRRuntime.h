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
#include "LLVMInclude.h"

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

        //
        // Posix functions
        //
        IRPosixRuntime& GetPosixEmitter() { return _posixRuntime; }

    private:
        friend IRModuleEmitter;
        IRRuntime(IRModuleEmitter& module);
        
        std::string GetNamespacePrefix() const;

        //
        // Getting pointers to functions
        //

        // time
        llvm::Function* GetCurrentTimeFunction(); // returns a double containing the current time (in _milliseconds_ from some arbitrary start time)
        llvm::Function* ResolveCurrentTimeFunction(llvm::StructType* timespecType);

        // math
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

        // POSIX threads
        // pthread_create
        // pthread_equal
        // pthread_exit
        // pthread_join
        // pthread_self
        // pthread_mutex_init
        // pthread_mutex_destroy
        // pthread_mutex_lock
        // pthread_mutex_trylock
        // pthread_mutex_unlock
        // pthread_cond_init
        // pthread_cond_destroy
        // phtread_cond_wait
        // pthread_cond_timedwait
        // pthread_code_signal
        // pthread_cond_broadcast
        // pthread_once

        IRModuleEmitter& _module;
        IRPosixRuntime _posixRuntime;

        llvm::Function* _pDotProductFunctionFloat = nullptr;
        llvm::Function* _pDotProductFunction = nullptr;
        llvm::Function* _pGetCurrentTimeFunction = nullptr;
    };
}
}

#include "../tcc/IRRuntime.tcc"
