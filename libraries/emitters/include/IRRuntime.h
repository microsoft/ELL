////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRRuntime.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRFunctionEmitter.h"

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
        IRRuntime(IRModuleEmitter& module);

        /// <summary> Get the dot product function for integers </summary>
        llvm::Function* GetDotProductFunction();

        /// <summary> Get the dot product function for floating point </summary>
        llvm::Function* GetDotProductFloatFunction();

        /// <summary> Get the sqrt function </summary>
        template <typename ValueType>
        llvm::Function* GetSqrtFunction();

        /// <summary> Get the abs function </summary>
        template <typename ValueType>
        llvm::Function* GetAbsFunction();

    private:
        llvm::Function* GetSqrtFunction(VariableType argType);
        llvm::Function* GetAbsFunction(VariableType argType);

        llvm::Function* EmitDotProductFunction();
        llvm::Function* EmitDotProductFunctionF();

        IRModuleEmitter& _module;
        NamedVariableTypeList _arguments;
        llvm::Function* _pDotProductFunctionFloat = nullptr;
        llvm::Function* _pDotProductFunction = nullptr;
    };
}
}

#include "../tcc/IRRuntime.tcc"
