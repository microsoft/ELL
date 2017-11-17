////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRTask.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRThreadPool.h"
#include "IRAsyncTask.h"

// stl
#include <vector>

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;

    //
    // Simple task class for asynchronous / parallelizable tasks
    //

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
    // 2) Run them
    //
    // auto task1Args = std::vector<llvm::Value*> {arg1, arg2, arg3};
    // ...
    // auto tasks = function.StartTasks(function, {{task1Args}, {task2Args}, {task3Args}, ...}); // Runs on the default thread pool for the module
    //
    // 3) Wait for tasks to finish
    //
    // tasks.WaitAll(); // block until all tasks are done
    //
    
    /// <summary> Class representing tasks that can be run asynchronously. </summary>
    class IRTask
    {
    public:
        /// <summary> Copy constructor </summary>
        IRTask(const IRTask& other);

        /// <summary> Destructor </summary>
        ~IRTask();

        /// <summary> Wait for the task to finish. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        void Wait(IRFunctionEmitter& function);
        
        /// <summary> Get the return value of a finished task </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        llvm::Value* GetReturnValue(IRFunctionEmitter& function);

        /// <summary> Check if a task is a "null task" </summary>
        llvm::Value* IsNull(IRFunctionEmitter& function);

    private:
        friend class IRTaskArray;
        friend class IRFunctionEmitter;
        enum class TaskType { async, threadPool };
        TaskType _type;
        union TaskUnion
        {
            TaskUnion(const IRAsyncTask& task) : asyncTask(task) {}
            TaskUnion(const IRThreadPoolTask& task) : threadPoolTask(task) {}
            ~TaskUnion(){}
            
            IRAsyncTask asyncTask;
            IRThreadPoolTask threadPoolTask;
        } _task;

        // Constructor from an IRAsyncTask
        IRTask(const IRAsyncTask& asyncTask);
        
        // Constructor from an IRThreadPoolTask
        IRTask(const IRThreadPoolTask& threadPoolTask);
    };

    //
    // IRTaskArray
    //

    /// <summary> Class representing a set of tasks that share the same task function, but have different arguments. </summary>
    class IRTaskArray
    {
    public:
        /// <summary> Copy constructor </summary>
        IRTaskArray(const IRTaskArray& other);

        /// <summary> Destructor. </summary>
        ~IRTaskArray();

        /// <summary> Wait for all tasks to finish. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        void WaitAll(IRFunctionEmitter& function);

        /// <summary> Get a specific task. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        /// <param name="taskIndex"> The index of the task to get. </param>
        ///
        /// <returns> The task specified by the index. </returns>
        IRTask GetTask(IRFunctionEmitter& function, size_t taskIndex);

    private:
        friend class IRFunctionEmitter;
        IRTask::TaskType _type;
        union TasksUnion
        {
            TasksUnion(const std::vector<IRAsyncTask>& tasks) : asyncTasks(tasks) {}
            TasksUnion(const IRThreadPoolTaskArray& tasks) : threadPoolTasks(tasks) {}
            ~TasksUnion(){}
            
            std::vector<IRAsyncTask> asyncTasks;
            IRThreadPoolTaskArray threadPoolTasks;
        } _tasks;

        IRTaskArray(const std::vector<IRAsyncTask>& asyncTasks);
        IRTaskArray(const IRThreadPoolTaskArray& threadPoolTasks);
    };

}
}
