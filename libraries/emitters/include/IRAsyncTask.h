////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRAsyncTask.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IREmitter.h"

#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;

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
    // task.Sync(); // block until task is done

    /// <summary> Class that emits functions as asynchronous tasks. </summary>
    class IRAsyncTask
    {
    public:
        /// <summary> Run this task. </summary>
        void Run();

        /// <summary> Wait for this task to finish. </summary>
        void Sync();

        /// <summary> Get the return value of task </summary>
        llvm::Value* GetReturnValue() const { return _returnValue; }

    private:
        friend IRFunctionEmitter;
        IRAsyncTask(IRFunctionEmitter& functionEmitter, llvm::Function* taskFunction, const std::vector<llvm::Value*>& arguments);
        IRAsyncTask(IRFunctionEmitter& functionEmitter, IRFunctionEmitter& taskFunction, const std::vector<llvm::Value*>& arguments);

        void StartTask();
        llvm::StructType* GetTaskArgStructType();
        llvm::Function* GetPthreadWrapper(llvm::StructType* argsStructType);

        IRFunctionEmitter& _owningFunction; // Loop written into this function
        llvm::Function* _taskFunction = nullptr;
        std::vector<llvm::Value*> _arguments;
        llvm::Value* _returnValue = nullptr;
        bool _started = false;

        // Pthread implementation
        llvm::Value* _pthread = nullptr;
        llvm::Value* _statusVar = nullptr;
    };

    // Threadpool notes:
    // We can make a constrained threadpool that doesn't require any dynamic allocation or growable data structures on our part.
    //
    // Conditions:
    // * always schedule a block (array) of tasks at once, size known at compile time
    // * can only schedule new tasks when the task queue is empty (all previous tasks are done)
    //
    // The task queue is just the array of original tasks
    // plus, we need to keep track of 3 ints:
    // * size of task queue  (don't even need this, except for bounds-checking access)
    // * index of next task to schedule
    // * number of unfinished tasks (either running or not scheduled yet) -- when this is zero, threadpool is idle again

    // Threadpool
    //   - Init(# tasks)  -- have this be called during program initialization, using LLVM's 'constructor' thing
    //   - ScheduleTasks(Task[])
    //   - WaitAll()
    //
    //
    // Init(N)
    // {
    //   create condition variable
    //   create N threads
    //   start threads
    // }
    //
    // ScheduleTasks(Task[] ts)
    // {
    //   lock(queueMutex)
    //     assert(count == 0)
    //     _taskQueue = ts
    //     _count = ts.size()
    //     _nextTaskIndex = 0
    //   unlock(queueMutex)
    // }
    //
    // WaitAll()
    // {
    //   wait on condition variable for _count == 0
    // }
    //
    //
    // TaskThread loop:
    // while(true)
    // {
    //   WaitForWork() // wait on a condition variable for some work to be scheduled
    //   while(true)
    //   {
    //    if(count == 0) { break; } // Maybe we can just do this here, outside the mutex
    //    lock(queueMutex)
    //    if(count == 0) { unlock(queueMutex); break; } // ... or maybe we have to do it here, if the read isn't safe to do w/out a mutex
    //    thisTaskIndex = nextTaskIndex++;
    //    unlock(queueMutex)
    //    t = taskQueue[thisTaskIndex]
    //    call t()
    //    lock(queueMutex)
    //    mycount = --count
    //    unlock(queueMutex)
    //    if(mycount == 0) {signal condition variable; break;}
    //   }
    // }
}
}
