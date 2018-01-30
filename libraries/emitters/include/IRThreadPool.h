////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRThreadPool.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IREmitter.h"

// llvm
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

// stl
#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;
    class IRModuleEmitter;

    class IRThreadPoolTaskQueue;
    class IRThreadPoolTaskArray;

    //
    // IRThreadPool: Simple thread pool class that schedules tasks in blocks, and associated classes:
    //
    // IRThreadPoolTask
    // IRThreadPoolTaskArray
    // IRThreadPoolTaskQueue
    // IRThreadPool
    //

    //
    // IRThreadPoolTask
    //

    /// <summary> Class representing tasks that can be run asynchronously. </summary>
    class IRThreadPoolTask
    {
    public:
        /// <summary> Run the task </summary>
        void Run(IRFunctionEmitter& function);

        /// <summary> Wait for the task to finish. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        void Wait(IRFunctionEmitter& function);

        /// <summary> Get the return value of a finished task </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        llvm::Value* GetReturnValue(IRFunctionEmitter& function);

        /// <summary> Check if a task is a "null task" </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        llvm::Value* IsNull(IRFunctionEmitter& function);

    private:
        friend class IRThreadPoolTaskArray;
        friend class IRThreadPoolTaskQueue;
        IRThreadPoolTask(llvm::Value* wrappedTaskFunctionPtr, llvm::Value* argsStructPtr, llvm::Value* returnValuePtr, IRThreadPoolTaskArray* taskArray);

        llvm::Value* _taskFunctionPtr = nullptr;
        llvm::Value* _argsStruct = nullptr;
        llvm::Value* _returnValuePtr = nullptr;

        IRThreadPoolTaskArray* _taskArray = nullptr;
    };

    //
    // IRThreadPoolTaskArray
    //

    /// <summary> Class representing a set of asynchronous tasks that share the same task function, but have different arguments. </summary>
    class IRThreadPoolTaskArray
    {
    public:
        /// <summary> Wait for all tasks to finish. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        void WaitAll(IRFunctionEmitter& function);

        /// <summary> Get a specific task. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        /// <param name="taskIndex"> The index of the task to get. </param>
        IRThreadPoolTask GetTask(IRFunctionEmitter& function, size_t taskIndex);

        /// <summary> Get a specific task. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        /// <param name="taskIndex"> The index of the task to get. </param>
        IRThreadPoolTask GetTask(IRFunctionEmitter& function, llvm::Value* taskIndex);

    private:
        friend class IRThreadPoolTaskQueue;
        IRThreadPoolTaskArray(IRThreadPoolTaskQueue& taskQueue);
        void Initialize(IRFunctionEmitter& function);
        void SetTasks(IRFunctionEmitter& function, llvm::Function* taskFunction, const std::vector<std::vector<llvm::Value*>>& taskArgs);
        llvm::StructType* GetTaskArrayDataType(IRModuleEmitter& module);
        llvm::Value* GetTaskFunctionPointer(IRFunctionEmitter& function);
        llvm::Value* GetReturnValuesStoragePointer(IRFunctionEmitter& function);
        llvm::Value* GetTaskArgsStoragePointer(IRFunctionEmitter& function);
        llvm::Value* GetTaskArgsStructSize(IRFunctionEmitter& function);
        void SetTaskArgsStructSize(IRFunctionEmitter& function, llvm::Value* size);

        enum class Fields
        {
            functionPtr = 0,
            returnValues,
            argStorage,
            argStructSize,
            // nextArray
        };
        llvm::Value* _taskArrayData = nullptr;

        IRThreadPoolTaskQueue& _taskQueue; // make this a pointer if we're not using threadpool
    };

    //
    // IRThreadPoolTaskQueue
    //

    /// <summary> Class representing a queue of tasks to be scheduled and run. </summary>
    class IRThreadPoolTaskQueue
    {
    public:
        /// <summary> Starts an array of tasks in the thread pool. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        /// <param name="taskFunction"> The function to run asynchronously with many different arguments. </param>
        /// <param name="arguments"> For each task, a vector of arguments for that task. </param>
        ///
        /// <returns> A task array object representing the running tasks. </param>
        IRThreadPoolTaskArray& StartTasks(IRFunctionEmitter& function, llvm::Function* taskFunction, const std::vector<std::vector<llvm::Value*>>& arguments);

        /// <summary> Pop a task off the task queue, waiting for one to become available if necessary. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        ///
        /// <returns> The next task in the queue. </param>
        IRThreadPoolTask PopNextTask(IRFunctionEmitter& function);

        /// <summary> Wait for all tasks to finish. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        void WaitAll(IRFunctionEmitter& function);

        /// <summary> Gets the array of tasks in the thread pool. </summary>
        ///
        /// <returns> A task array object representing the tasks. </param>
        IRThreadPoolTaskArray& GetTaskArray() { return _tasks; }

    private:
        friend class IRThreadPool;
        IRThreadPoolTaskQueue(); // create an empty queue
        void Initialize(IRFunctionEmitter& function); // initializes the task array
        llvm::Value* GetDataStruct() { return _queueData; }
        llvm::Value* DecrementCountField(IRFunctionEmitter& function, llvm::Value* fieldPtr);
        llvm::StructType* GetTaskQueueDataType(IRModuleEmitter& module);

        // Accessors for fields
        llvm::Value* GetQueueMutexPointer(IRFunctionEmitter& function);
        llvm::Value* GetWorkAvailableConditionVariablePointer(IRFunctionEmitter& function);
        llvm::Value* GetWorkFinishedConditionVariablePointer(IRFunctionEmitter& function);
        llvm::Value* GetUnscheduledCount(IRFunctionEmitter& function);
        llvm::Value* GetUnfinishedCount(IRFunctionEmitter& function);
        void SetShutdownFlag(IRFunctionEmitter& function);
        llvm::Value* GetShutdownFlag(IRFunctionEmitter& function);
        void SetInitialCount(IRFunctionEmitter& function, llvm::Value* newValue);
        llvm::Value* DecrementUnscheduledTasks(IRFunctionEmitter& function);
        llvm::Value* DecrementUnfinishedTasks(IRFunctionEmitter& function);
        llvm::Value* IsEmpty(IRFunctionEmitter& function);
        llvm::Value* IsFinished(IRFunctionEmitter& function);

        bool IsInitialized();
        void NotifyWaitingClients(IRFunctionEmitter& function);
        void LockQueueMutex(IRFunctionEmitter& function);
        void UnlockQueueMutex(IRFunctionEmitter& function);
        void ShutDown(IRFunctionEmitter& function);

        enum class Fields
        {
            queueMutex = 0,
            workAvailableCondVar,
            workFinishedCondVar,
            unscheduledCount,
            unfinishedCount,
            shutdownFlag
        };
        llvm::Value* _queueData = nullptr; // a struct with the above fields
        IRThreadPoolTaskArray _tasks;
    };

    //
    // IRThreadPool
    //

    /// <summary> Class representing a set of threads that can run asynchronous tasks. </summary>
    class IRThreadPool
    {
    public:
        /// <summary> Constructor. </summary>
        ///
        /// <param name="module"> The module being emitted. </param>
        IRThreadPool(IRModuleEmitter& module);

        /// <summary> Starts an array of tasks in the thread pool. </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        /// <param name="taskFunction"> The function to run asynchronously with many different arguments. </param>
        /// <param name="arguments"> For each task, a vector of arguments for that task. </param>
        ///
        /// <returns> A task array object representing the running tasks. </param>
        IRThreadPoolTaskArray& AddTasks(IRFunctionEmitter& function, llvm::Function* taskFunction, const std::vector<std::vector<llvm::Value*>>& arguments);

        /// <summary> Tell the thread pool to finish and kill the treads. </summary>
        void ShutDown(IRFunctionEmitter& function);

    private:
        void Initialize(); // Allocates threads and adds global initializer and finalizer functions
        bool IsInitialized();
        void AddGlobalInitializer();
        void AddGlobalFinalizer();
        llvm::Function* GetWorkerThreadFunction();

        IRModuleEmitter& _module;
        size_t _maxThreads = 0;
        llvm::GlobalVariable* _threads = nullptr; // global array of pthread_t

        // task queue
        IRThreadPoolTaskQueue _taskQueue;
    };
}
}
