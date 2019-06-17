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

#include <utilities/include/Exception.h>

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

        /// <summary> Get the pow function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetPowFunction();

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

        /// <summary> Get the copysign function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetCopySignFunction();

        /// <summary> Get the tanh function </summary>
        ///
        /// <returns> An LLVM function pointer to the function. </returns>
        template <typename ValueType>
        LLVMFunction GetTanhFunction();

        // emitter types
        LLVMFunction GetSqrtFunction(VariableType argType);
        LLVMFunction GetAbsFunction(VariableType argType);
        LLVMFunction GetExpFunction(VariableType argType);
        LLVMFunction GetPowFunction(VariableType argType);
        LLVMFunction GetLogFunction(VariableType argType);
        LLVMFunction GetLog10Function(VariableType argType);
        LLVMFunction GetLog2Function(VariableType argType);
        LLVMFunction GetTanhFunction(VariableType argType);
        LLVMFunction GetSinFunction(VariableType argType);
        LLVMFunction GetCosFunction(VariableType argType);
        LLVMFunction GetRoundFunction(VariableType argType);
        LLVMFunction GetFloorFunction(VariableType argType);
        LLVMFunction GetCeilFunction(VariableType argType);
        LLVMFunction GetCopySignFunction(VariableType argType);

        // llvm types
        LLVMFunction GetSqrtFunction(LLVMType argType);
        LLVMFunction GetAbsFunction(LLVMType argType);
        LLVMFunction GetExpFunction(LLVMType argType);
        LLVMFunction GetPowFunction(LLVMType argType);
        LLVMFunction GetLogFunction(LLVMType argType);
        LLVMFunction GetLog10Function(LLVMType argType);
        LLVMFunction GetLog2Function(LLVMType argType);
        LLVMFunction GetTanhFunction(LLVMType argType);
        LLVMFunction GetSinFunction(LLVMType argType);
        LLVMFunction GetCosFunction(LLVMType argType);
        LLVMFunction GetRoundFunction(LLVMType argType);
        LLVMFunction GetFloorFunction(LLVMType argType);
        LLVMFunction GetCeilFunction(LLVMType argType);
        LLVMFunction GetCopySignFunction(LLVMType argType);

        LLVMFunction GetPrefetchFunction();

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

#pragma region implementation

namespace ell
{
namespace emitters
{
    template <typename ValueType>
    LLVMFunction IRRuntime::GetSqrtFunction()
    {
        return GetSqrtFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetAbsFunction()
    {
        return GetAbsFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetExpFunction()
    {
        return GetExpFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetPowFunction()
    {
        return GetPowFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetLogFunction()
    {
        return GetLogFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetTanhFunction()
    {
        return GetTanhFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetSinFunction()
    {
        return GetSinFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetCosFunction()
    {
        return GetCosFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetCopySignFunction()
    {
        return GetCopySignFunction(GetVariableType<ValueType>());
    }

    template <typename ValueType>
    LLVMFunction IRRuntime::GetDotProductFunction()
    {
        if (std::is_integral<std::decay_t<ValueType>>::value)
        {
            if (_dotProductFunction == nullptr)
            {
                _dotProductFunction = GetDotProductIntFunction();
            }
            return _dotProductFunction;
        }
        else if (std::is_floating_point<std::decay_t<ValueType>>::value)
        {
            if (_dotProductFunctionFloat == nullptr)
            {
                _dotProductFunctionFloat = GetDotProductFloatFunction();
            }
            return _dotProductFunctionFloat;
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }
} // namespace emitters
} // namespace ell

#pragma endregion implementation
