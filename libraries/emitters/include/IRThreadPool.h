////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRThreadPool.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IREmitter.h"
#include "LLVMUtilities.h"

// llvm
#include <llvm/IR/GlobalVariable.h>

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
        LLVMValue GetReturnValue(IRFunctionEmitter& function);

        /// <summary> Check if a task is a "null task" </summary>
        ///
        /// <param name="function"> The function currently being emitted into. </param>
        LLVMValue IsNull(IRFunctionEmitter& function);

    private:
        friend class IRThreadPoolTaskArray;
        friend class IRThreadPoolTaskQueue;
        IRThreadPoolTask(LLVMValue wrappedTaskFunctionPtr, LLVMValue argsStructPtr, LLVMValue returnValuePtr, IRThreadPoolTaskArray* taskArray);

        LLVMValue _taskFunctionPtr = nullptr;
        LLVMValue _argsStruct = nullptr;
        LLVMValue _returnValuePtr = nullptr;

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
        IRThreadPoolTask GetTask(IRFunctionEmitter& function, LLVMValue taskIndex);

    private:
        friend class IRThreadPoolTaskQueue;
        IRThreadPoolTaskArray(IRThreadPoolTaskQueue& taskQueue);
        void Initialize(IRFunctionEmitter& function);
        void SetTasks(IRFunctionEmitter& function, LLVMFunction taskFunction, const std::vector<std::vector<LLVMValue>>& taskArgs);
        llvm::StructType* GetTaskArrayDataType(IRModuleEmitter& module);
        LLVMValue GetTaskFunctionPointer(IRFunctionEmitter& function);
        LLVMValue GetReturnValuesStoragePointer(IRFunctionEmitter& function);
        LLVMValue GetTaskArgsStoragePointer(IRFunctionEmitter& function);
        LLVMValue GetTaskArgsStructSize(IRFunctionEmitter& function);
        void SetTaskArgsStructSize(IRFunctionEmitter& function, LLVMValue size);

        enum class Fields
        {
            functionPtr = 0,
            returnValues,
            argStorage,
            argStructSize,
            // nextArray
        };
        LLVMValue _taskArrayData = nullptr;

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
        IRThreadPoolTaskArray& StartTasks(IRFunctionEmitter& function, LLVMFunction taskFunction, const std::vector<std::vector<LLVMValue>>& arguments);

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
        LLVMValue GetDataStruct() { return _queueData; }
        LLVMValue DecrementCountField(IRFunctionEmitter& function, LLVMValue fieldPtr);
        llvm::StructType* GetTaskQueueDataType(IRModuleEmitter& module) const;

        // Accessors for fields
        LLVMValue GetQueueMutexPointer(IRFunctionEmitter& function);
        LLVMValue GetWorkAvailableConditionVariablePointer(IRFunctionEmitter& function);
        LLVMValue GetWorkFinishedConditionVariablePointer(IRFunctionEmitter& function);
        LLVMValue GetUnscheduledCount(IRFunctionEmitter& function) const;
        LLVMValue GetUnfinishedCount(IRFunctionEmitter& function) const;
        void SetShutdownFlag(IRFunctionEmitter& function);
        LLVMValue GetShutdownFlag(IRFunctionEmitter& function) const;
        void SetInitialCount(IRFunctionEmitter& function, LLVMValue newValue);
        LLVMValue DecrementUnscheduledTasks(IRFunctionEmitter& function);
        LLVMValue DecrementUnfinishedTasks(IRFunctionEmitter& function);
        LLVMValue IsEmpty(IRFunctionEmitter& function) const;
        LLVMValue IsFinished(IRFunctionEmitter& function) const;

        bool IsInitialized() const;
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
        LLVMValue _queueData = nullptr; // a struct with the above fields
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
        IRThreadPoolTaskArray& AddTasks(IRFunctionEmitter& function, LLVMFunction taskFunction, const std::vector<std::vector<LLVMValue>>& arguments);

        /// <summary> Tell the thread pool to finish and kill the treads. </summary>
        void ShutDown(IRFunctionEmitter& function);

    private:
        void Initialize(); // Allocates threads and adds global initializer and finalizer functions
        bool IsInitialized() const;
        void AddGlobalInitializer();
        void AddGlobalFinalizer();
        LLVMFunction GetWorkerThreadFunction();

        IRModuleEmitter& _module;
        size_t _maxThreads = 0;
        llvm::GlobalVariable* _threads = nullptr; // global array of pthread_t

        // task queue
        IRThreadPoolTaskQueue _taskQueue;
    };
}
}
