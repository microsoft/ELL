////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRThreadUtilities.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRModuleEmitter.h"
#include "IRFunctionEmitter.h"

// llvm
#include <llvm/IR/Function.h>

// stl
#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    //
    // Functions
    //

    /// <summary> Get a wrapper function suitable for passing as a thread worker function (with signature `void*(*)(void*)`)
    ///
    /// <param name="module"> The module being emitted. </param>
    /// <param name="taskFunction"> The function implementing the task. </param>
    ///
    /// <returns> A function that takes a single struct argument and forwards the fields as arguments to the given task function. </returns>
    llvm::Function* GetTaskWrapperFunction(IRModuleEmitter& module, llvm::Function* taskFunction);
    
    /// <summary> Get a wrapper function suitable for passing as a thread worker function (with signature `void*(*)(void*)`)
    ///
    /// <param name="module"> The module being emitted. </param>
    /// <param name="taskFunction"> The function implementing the task. </param>
    ///
    /// <returns> A function that takes a single struct argument and forwards the fields as arguments to the given task function. </returns>
    llvm::Function* GetTaskWrapperFunction(IRModuleEmitter& module, IRFunctionEmitter& taskFunction);

    /// <summary> Get an LLVM struct type suitable for holding the functions arguments </summary>
    ///
    /// <param name="module"> The module being emitted. </param>
    /// <param name="taskFunction"> The function implementing the task. </param>
    ///
    /// <returns> An LLVM StructType pointer for a struct that can hold the functions arguments. </returns>
    llvm::StructType* GetTaskArgStructType(IRModuleEmitter& module, llvm::Function* taskFunction);
}
}
