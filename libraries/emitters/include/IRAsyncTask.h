////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRAsyncTask.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LLVMUtilities.h"

// stl
#include <vector>

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;
    class IRModuleEmitter;

    // Usage:
    //
    // 1) Define task function
    //
    // auto taskFunction = module.BeginFunction("taskFunction", {argType1, argType2});
    // {
    //   auto arg0 = taskFunction.GetArgument(0);
    //   auto arg1 = taskFunction.GetArgument(1);
    //   // ...
    //   taskFunction.Return(...);
    // }
    // taskFunction.End();
    //
    // 2) Schedule it:
    //
    // ...
    // auto task = function.Async(taskFunction, {x0, x1});
    // ...
    // task.Wait(function); // block until task is done

    /// <summary> Class that emits functions as asynchronous tasks. </summary>
    class IRAsyncTask
    {
    public:
        /// <summary> Run this task. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        void Run(IRFunctionEmitter& function);

        /// <summary> Wait for this task to finish. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        void Wait(IRFunctionEmitter& function);

        /// <summary> Get the return value of task </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        LLVMValue GetReturnValue(IRFunctionEmitter& function) const;

        /// <summary> Wait for this task to finish. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        LLVMValue IsNull(IRFunctionEmitter& function);

    private:
        friend IRFunctionEmitter;
        IRAsyncTask(IRFunctionEmitter& functionEmitter, LLVMFunction taskFunction, const std::vector<LLVMValue>& arguments);
        IRAsyncTask(IRFunctionEmitter& functionEmitter, IRFunctionEmitter& taskFunction, const std::vector<LLVMValue>& arguments);

        LLVMFunction _taskFunction = nullptr;
        std::vector<LLVMValue> _arguments;
        LLVMType _returnType = nullptr;
        LLVMValue _returnValue = nullptr;

        bool UsePthreads() const { return _usePthreads; }
        LLVMFunction GetPthreadWrapper(llvm::StructType* argsStructType);

        bool _usePthreads = false;

        // For pthreads implementation
        LLVMValue _pthread = nullptr;
    };

    /// <summary> Waits for all given tasks to finish </summary>
    void SyncAllTasks(IRFunctionEmitter& function, std::vector<IRAsyncTask>& tasks);
} // namespace emitters
} // namespace ell
