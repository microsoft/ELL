////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRThreadPool.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRThreadPool.h"
#include "EmitterException.h"
#include "IRFunctionEmitter.h"
#include "IRHeaderWriter.h"
#include "IRLoopEmitter.h"
#include "IRModuleEmitter.h"
#include "IRThreadUtilities.h"

// utilities
#include "Exception.h"
#include "Unused.h"

// stl
#include <iostream>

namespace ell
{
namespace emitters
{
    //
    // IRThreadPool
    //

    IRThreadPool::IRThreadPool(IRModuleEmitter& module)
        : _module(module)
    {
    }

    void IRThreadPool::Initialize()
    {
        _maxThreads = _module.GetCompilerParameters().maxThreads;
        auto pthreadType = _module.GetRuntime().GetPosixEmitter().GetPthreadType();

        // Create global array to hold pthread objects
        _threads = _module.GlobalArray("taskThreads", pthreadType, _maxThreads);

        AddGlobalInitializer();
        AddGlobalFinalizer();
    }

    void IRThreadPool::AddGlobalInitializer()
    {
        auto& context = _module.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto boolType = llvm::Type::getInt1Ty(context);

        // Create individual threads (in a global_ctors function)
        auto initThreadPoolFunction = _module.BeginFunction("initThreadPool", voidType);
        {
            auto isInitedVar = _module.Global(boolType, "isInitialized"); // initialized to false
            auto ife = initThreadPoolFunction.If(); // check if task OK
            ife.If(initThreadPoolFunction.LogicalNot(initThreadPoolFunction.Load(isInitedVar)));
            {
                initThreadPoolFunction.Store(isInitedVar, initThreadPoolFunction.TrueBit());
                _taskQueue.Initialize(initThreadPoolFunction);

                auto workerThreadFunction = GetWorkerThreadFunction();
                llvm::ConstantPointerNull* nullAttr = initThreadPoolFunction.NullPointer(int8PtrType);
                auto loop = initThreadPoolFunction.ForLoop();
                loop.Begin(_maxThreads);
                {
                    auto index = loop.LoadIterationVariable();
                    auto threadPtr = initThreadPoolFunction.PointerOffset(_threads, index);
                    initThreadPoolFunction.PthreadCreate(threadPtr, nullAttr, workerThreadFunction, initThreadPoolFunction.CastPointer(_taskQueue.GetDataStruct(), int8PtrType));
                }
                loop.End();
            }
            ife.End();
        }
        _module.EndFunction();
        _module.AddInitializationFunction(initThreadPoolFunction);
    }

    void IRThreadPool::AddGlobalFinalizer()
    {
        auto& context = _module.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);

        // Create individual threads (in a global_ctors function)
        auto shutDownThreadPoolFunction = _module.BeginFunction("shutDownThreadPool", voidType);
        {
            ShutDown(shutDownThreadPoolFunction);
        }
        _module.EndFunction();
        _module.AddFinalizationFunction(shutDownThreadPoolFunction);
    }

    IRThreadPoolTaskArray& IRThreadPool::AddTasks(IRFunctionEmitter& function, llvm::Function* taskFunction, const std::vector<std::vector<llvm::Value*>>& arguments)
    {
        // Call Initialize() the first time we're called --- this adds global init code to the module
        static bool initialized = false;
        if (!initialized)
        {
            Initialize();
            initialized = true;
        }

        return _taskQueue.StartTasks(function, taskFunction, arguments);
    }

    void IRThreadPool::ShutDown(IRFunctionEmitter& function)
    {
        auto& context = function.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);

        _taskQueue.ShutDown(function);

        // Now wait for the worker threads to finish
        auto loop = function.ForLoop();
        loop.Begin(_maxThreads);
        {
            auto index = loop.LoadIterationVariable();
            auto threadPtr = function.PointerOffset(_threads, index);
            function.PthreadJoin(function.Load(threadPtr), function.NullPointer(int8PtrType->getPointerTo()));
        }
        loop.End();
    }

    llvm::Function* IRThreadPool::GetWorkerThreadFunction()
    {
        assert(IsInitialized());

        auto& context = _module.GetLLVMContext();
        auto boolType = llvm::Type::getInt1Ty(context);
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);

        auto workerThreadFunction = _module.BeginFunction("WorkerThreadFunction", int8PtrType, { int8PtrType });
        {
            auto notDoneVar = workerThreadFunction.Variable(boolType, "notDone");
            workerThreadFunction.Store(notDoneVar, workerThreadFunction.TrueBit());
            auto loop = workerThreadFunction.WhileLoop();
            loop.Begin(notDoneVar);
            {
                auto task = _taskQueue.PopNextTask(workerThreadFunction);
                auto ife = workerThreadFunction.If(); // check for a poison "null" task, indicating we should break out of the loop and terminate the thread
                ife.If(task.IsNull(workerThreadFunction));
                {
                    workerThreadFunction.Store(notDoneVar, workerThreadFunction.FalseBit());
                }
                ife.Else();
                {
                    task.Run(workerThreadFunction);

                    // Decrement count of unfinished tasks
                    _taskQueue.LockQueueMutex(workerThreadFunction);
                    auto unfinishedCount = _taskQueue.DecrementUnfinishedTasks(workerThreadFunction);
                    _taskQueue.UnlockQueueMutex(workerThreadFunction);

                    // if zero, signal client cond var
                    auto isDoneIf = workerThreadFunction.If();
                    isDoneIf.If(TypedComparison::equals, unfinishedCount, workerThreadFunction.Literal<int>(0));
                    {
                        _taskQueue.NotifyWaitingClients(workerThreadFunction);
                    }
                    isDoneIf.End();
                }
                ife.End();
            }
            loop.End();

            workerThreadFunction.Return(workerThreadFunction.NullPointer(int8PtrType));
        }
        _module.EndFunction();
        return workerThreadFunction.GetFunction();
    }

    bool IRThreadPool::IsInitialized()
    {
        return _threads != nullptr;
    }

    //
    // IRThreadPoolTaskQueue
    //

    IRThreadPoolTaskQueue::IRThreadPoolTaskQueue()
        : _tasks(*this)
    {
        // Note: we can't initialize ourselves here, for ordering reasons.
    }

    void IRThreadPoolTaskQueue::Initialize(IRFunctionEmitter& function)
    {
        if(_queueData != nullptr)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Error: initializing thread pool task queue more than once");
        }
        auto& module = function.GetModule();

        // Get types
        auto& context = module.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto taskQueueDataType = GetTaskQueueDataType(module);

        // Allocate a data struct
        _queueData = module.Global(taskQueueDataType, "taskQueueData");

        // Get pointers to the fields
        auto queueMutex = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::queueMutex));
        auto workAvailableCondVar = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::workAvailableCondVar));
        auto workFinishedCondVar = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::workFinishedCondVar));
        auto count = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::unscheduledCount));
        auto unfinishedCount = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::unfinishedCount));
        auto shutdownFlag = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::shutdownFlag));

        // Initialize the fields
        llvm::ConstantPointerNull* nullAttr = function.NullPointer(int8PtrType);
        llvm::Value* errCode = function.PthreadMutexInit(queueMutex, nullAttr);
        errCode = function.PthreadCondInit(workAvailableCondVar, nullAttr);
        errCode = function.PthreadCondInit(workFinishedCondVar, nullAttr);
        UNUSED(errCode);
        function.Store(count, function.Literal<int>(0));
        function.Store(unfinishedCount, function.Literal<int>(0));
        function.Store(shutdownFlag, function.FalseBit());

        _tasks.Initialize(function);
    }

    IRThreadPoolTaskArray& IRThreadPoolTaskQueue::StartTasks(IRFunctionEmitter& function, llvm::Function* taskFunction, const std::vector<std::vector<llvm::Value*>>& arguments)
    {
        assert(IsInitialized());

        // TODO: assert we're idle (until we can handle multiple task arrays to be active)

        const auto numTasks = arguments.size();

        LockQueueMutex(function);
        _tasks.SetTasks(function, taskFunction, arguments);
        SetInitialCount(function, function.Literal<int>(numTasks));
        function.PthreadCondBroadcast(GetWorkAvailableConditionVariablePointer(function));
        UnlockQueueMutex(function);
        return GetTaskArray();
    }

    void IRThreadPoolTaskQueue::NotifyWaitingClients(IRFunctionEmitter& function)
    {
        function.PthreadCondBroadcast(GetWorkFinishedConditionVariablePointer(function));
    }

    IRThreadPoolTask IRThreadPoolTaskQueue::PopNextTask(IRFunctionEmitter& function)
    {
        assert(IsInitialized());

        auto& module = function.GetModule();
        auto& context = module.GetLLVMContext();
        auto boolType = llvm::Type::getInt1Ty(context);

        auto isEmptyVar = function.Variable(boolType, "isEmpty");
        auto queueMutex = GetQueueMutexPointer(function);
        auto workAvailableCondVar = GetWorkAvailableConditionVariablePointer(function);

        LockQueueMutex(function);
        function.Store(isEmptyVar, function.Operator(TypedOperator::logicalAnd, IsEmpty(function), function.Operator(UnaryOperationType::logicalNot, GetShutdownFlag(function))));
        IRWhileLoopEmitter whileLoop(function);
        whileLoop.Begin(isEmptyVar);
        {
            // Wait on the condition variable
            function.PthreadCondWait(workAvailableCondVar, queueMutex);

            // update while loop exit condition
            function.Store(isEmptyVar, function.Operator(TypedOperator::logicalAnd, IsEmpty(function), function.Operator(UnaryOperationType::logicalNot, GetShutdownFlag(function))));
        }
        whileLoop.End();
        // At loop exit, mutex is locked. Now decrement _count, unlock mutex, and return task at index _count
        auto newCount = DecrementUnscheduledTasks(function);
        UnlockQueueMutex(function);

        // Get task from task array --- passing in a negative number (which is what happens if the array was empty) returns a null task
        return _tasks.GetTask(function, newCount);
    }

    bool IRThreadPoolTaskQueue::IsInitialized()
    {
        return _queueData != nullptr;
    }

    llvm::Value* IRThreadPoolTaskQueue::IsEmpty(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        return function.Comparison(TypedComparison::equals, GetUnscheduledCount(function), function.Literal<int>(0));
    }

    llvm::Value* IRThreadPoolTaskQueue::IsFinished(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        return function.Comparison(TypedComparison::equals, GetUnfinishedCount(function), function.Literal<int>(0));
    }

    void IRThreadPoolTaskQueue::ShutDown(IRFunctionEmitter& function)
    {
        SetShutdownFlag(function);
        function.PthreadCondBroadcast(GetWorkAvailableConditionVariablePointer(function));
        // Now PopNextTask will emit null tasks
    }

    void IRThreadPoolTaskQueue::WaitAll(IRFunctionEmitter& function)
    {
        auto& module = function.GetModule();
        auto& context = module.GetLLVMContext();
        auto boolType = llvm::Type::getInt1Ty(context);

        auto isNotDoneVar = function.Variable(boolType, "isNotDone");
        auto mutex = GetQueueMutexPointer(function);
        auto workFinishedCondVar = GetWorkFinishedConditionVariablePointer(function);

        LockQueueMutex(function);
        function.Store(isNotDoneVar, function.Operator(UnaryOperationType::logicalNot, IsFinished(function)));
        IRWhileLoopEmitter whileLoop(function);
        whileLoop.Begin(isNotDoneVar);
        {
            function.PthreadCondWait(workFinishedCondVar, mutex);
            function.Store(isNotDoneVar, function.Operator(UnaryOperationType::logicalNot, IsFinished(function)));
        }
        whileLoop.End();
        UnlockQueueMutex(function);
    }

    llvm::StructType* IRThreadPoolTaskQueue::GetTaskQueueDataType(IRModuleEmitter& module) // TODO: come up with a naming convention for "class" structs like this
    {
        auto& context = module.GetLLVMContext();
        auto mutexType = module.GetRuntime().GetPosixEmitter().GetPthreadMutexType();
        auto conditionVarType = module.GetRuntime().GetPosixEmitter().GetPthreadCondType();
        auto boolType = llvm::Type::getInt1Ty(context);
        auto int32Type = llvm::Type::getInt32Ty(context);

        std::vector<llvm::Type*> fieldTypes = { mutexType, conditionVarType, conditionVarType, int32Type, int32Type, boolType };
        return module.GetAnonymousStructType(fieldTypes);
    }

    llvm::Value* IRThreadPoolTaskQueue::GetQueueMutexPointer(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        return function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::queueMutex));
    }

    llvm::Value* IRThreadPoolTaskQueue::GetWorkAvailableConditionVariablePointer(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        return function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::workAvailableCondVar));
    }

    llvm::Value* IRThreadPoolTaskQueue::GetWorkFinishedConditionVariablePointer(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        return function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::workFinishedCondVar));
    }

    llvm::Value* IRThreadPoolTaskQueue::GetUnscheduledCount(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        auto fieldPtr = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::unscheduledCount));
        return function.Load(fieldPtr);
    }

    llvm::Value* IRThreadPoolTaskQueue::GetUnfinishedCount(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        auto fieldPtr = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::unfinishedCount));
        return function.Load(fieldPtr);
    }

    void IRThreadPoolTaskQueue::SetInitialCount(IRFunctionEmitter& function, llvm::Value* numTasks)
    {
        assert(IsInitialized());
        function.Store(function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::unscheduledCount)), numTasks);
        function.Store(function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::unfinishedCount)), numTasks);
    }

    llvm::Value* IRThreadPoolTaskQueue::DecrementCountField(IRFunctionEmitter& function, llvm::Value* fieldPtr)
    {
        auto count = function.Load(fieldPtr);
        auto newCount = function.Operator(TypedOperator::subtract, count, function.Literal<int>(1));
        auto ife = function.If();
        ife.If(TypedComparison::notEquals, count, function.Literal<int>(0));
        {
            function.Store(fieldPtr, newCount);
        }
        ife.End();
        return newCount;
    }

    llvm::Value* IRThreadPoolTaskQueue::DecrementUnscheduledTasks(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        auto fieldPtr = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::unscheduledCount));
        return DecrementCountField(function, fieldPtr);
    }

    llvm::Value* IRThreadPoolTaskQueue::DecrementUnfinishedTasks(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        auto fieldPtr = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::unfinishedCount));
        return DecrementCountField(function, fieldPtr);
    }

    llvm::Value* IRThreadPoolTaskQueue::GetShutdownFlag(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        auto fieldPtr = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::shutdownFlag));
        return function.Load(fieldPtr);
    }

    void IRThreadPoolTaskQueue::SetShutdownFlag(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        auto fieldPtr = function.GetStructFieldPointer(_queueData, static_cast<int>(Fields::shutdownFlag));
        function.Store(fieldPtr, function.TrueBit());
    }

    void IRThreadPoolTaskQueue::LockQueueMutex(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        auto errCode = function.PthreadMutexLock(GetQueueMutexPointer(function));
        UNUSED(errCode);
    }

    void IRThreadPoolTaskQueue::UnlockQueueMutex(IRFunctionEmitter& function)
    {
        assert(IsInitialized());
        auto errCode = function.PthreadMutexUnlock(GetQueueMutexPointer(function));
        UNUSED(errCode);
    }

    //
    // IRThreadPoolTask
    //
    IRThreadPoolTask::IRThreadPoolTask(llvm::Value* wrappedTaskFunctionPtr, llvm::Value* argsStructPtr, llvm::Value* returnValuePtr, IRThreadPoolTaskArray* taskArray)
        : _taskFunctionPtr(wrappedTaskFunctionPtr), _argsStruct(argsStructPtr), _returnValuePtr(returnValuePtr), _taskArray(taskArray)
    {
        if (_taskFunctionPtr == nullptr)
        {
            throw EmitterException(EmitterError::nullFunction);
        }
    }

    void IRThreadPoolTask::Run(IRFunctionEmitter& function)
    {
        auto& context = function.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto taskFunctionType = llvm::FunctionType::get(int8PtrType, { int8PtrType }, false);
        auto taskFunctionPtr = function.BitCast(_taskFunctionPtr, taskFunctionType->getPointerTo());

        auto& emitter = function.GetEmitter();
        auto& irBuilder = emitter.GetIRBuilder();

        auto returnValue = irBuilder.CreateCall(taskFunctionType, taskFunctionPtr, { _argsStruct });
        function.Store(_returnValuePtr, returnValue);
    }

    void IRThreadPoolTask::Wait(IRFunctionEmitter& function)
    {
        _taskArray->WaitAll(function);
    }

    llvm::Value* IRThreadPoolTask::GetReturnValue(IRFunctionEmitter& function)
    {
        return function.Load(_returnValuePtr);
    }

    llvm::Value* IRThreadPoolTask::IsNull(IRFunctionEmitter& function)
    {
        auto& context = function.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        llvm::ConstantPointerNull* nullPtr = function.NullPointer(int8PtrType);
        return function.Comparison(TypedComparison::equals, _argsStruct, nullPtr);
    }

    //
    // IRThreadPoolTaskArray
    //

    IRThreadPoolTaskArray::IRThreadPoolTaskArray(IRThreadPoolTaskQueue& taskQueue)
        : _taskQueue(taskQueue)
    {
    }

    void IRThreadPoolTaskArray::Initialize(IRFunctionEmitter& function)
    {
        assert(_taskArrayData == nullptr);
        auto& module = function.GetModule();

        // Get types
        auto taskArrayDataType = GetTaskArrayDataType(module);

        // Allocate our data struct
        _taskArrayData = module.Global(taskArrayDataType, "taskArrayData");
    }

    llvm::StructType* IRThreadPoolTaskArray::GetTaskArrayDataType(IRModuleEmitter& module) // TODO: come up with a naming convention for "class" structs like this
    {
        auto& context = module.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto int8PtrPtrType = int8PtrType->getPointerTo();
        auto int32Type = llvm::Type::getInt32Ty(context);

        std::vector<llvm::Type*> fieldTypes = { int8PtrType, int8PtrPtrType, int8PtrType, int32Type };
        return module.GetAnonymousStructType(fieldTypes);
    }

    llvm::Value* IRThreadPoolTaskArray::GetTaskFunctionPointer(IRFunctionEmitter& function)
    {
        return function.GetStructFieldPointer(_taskArrayData, static_cast<int>(Fields::functionPtr));
    }

    llvm::Value* IRThreadPoolTaskArray::GetReturnValuesStoragePointer(IRFunctionEmitter& function)
    {
        return function.GetStructFieldPointer(_taskArrayData, static_cast<int>(Fields::returnValues));
    }

    llvm::Value* IRThreadPoolTaskArray::GetTaskArgsStoragePointer(IRFunctionEmitter& function)
    {
        return function.GetStructFieldPointer(_taskArrayData, static_cast<int>(Fields::argStorage));
    }

    llvm::Value* IRThreadPoolTaskArray::GetTaskArgsStructSize(IRFunctionEmitter& function)
    {
        auto ptr = function.GetStructFieldPointer(_taskArrayData, static_cast<int>(Fields::argStructSize));
        return function.Load(ptr);
    }

    void IRThreadPoolTaskArray::SetTaskArgsStructSize(IRFunctionEmitter& function, llvm::Value* size)
    {
        auto ptr = function.GetStructFieldPointer(_taskArrayData, static_cast<int>(Fields::argStructSize));
        function.Store(ptr, size);
    }

    void IRThreadPoolTaskArray::SetTasks(IRFunctionEmitter& function, llvm::Function* taskFunction, const std::vector<std::vector<llvm::Value*>>& taskArgs)
    {
        // Here we need to fill in 3 things:
        // 1) the pointer to the task function shared by all tasks
        // 2) the variable that records the size in memory for the task function arguments
        // 3) the array of task function arguments itself

        assert(_taskArrayData != nullptr);

        auto& module = function.GetModule();
        auto& context = function.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto int8PtrPtrType = int8PtrType->getPointerTo();

        auto numTasks = taskArgs.size();

        // Get pointers into struct fields
        auto taskFunctionPtr = GetTaskFunctionPointer(function);
        auto returnValuesStoragePtr = GetReturnValuesStoragePointer(function);
        auto taskArgStoragePtr = GetTaskArgsStoragePointer(function);

        // First check if we're adding null tasks, and deal with them specially
        if (!taskFunction)
        {
            function.Store(taskFunctionPtr, function.NullPointer(int8PtrType));
            function.Store(returnValuesStoragePtr, function.NullPointer(int8PtrPtrType));
            SetTaskArgsStructSize(function, function.Literal<int>(0));
            return;
        }

        auto argStructType = GetTaskArgStructType(function.GetModule(), taskFunction);

        auto wrappedTaskFunction = GetTaskWrapperFunction(function.GetModule(), taskFunction);
        function.Store(taskFunctionPtr, function.BitCast(wrappedTaskFunction, int8PtrType));

        auto returnValuesStorage = function.Variable(int8PtrType, numTasks);
        function.Store(returnValuesStoragePtr, returnValuesStorage);

        auto taskArgStorage = function.Variable(argStructType, numTasks);
        function.Store(taskArgStoragePtr, function.CastPointer(taskArgStorage, int8PtrType));

        const auto& dataLayout = module.GetTargetDataLayout();
        auto argStructSize = dataLayout.getTypeAllocSize(argStructType);
        SetTaskArgsStructSize(function, function.Literal<int>(argStructSize));

        // copy args to taskData struct
        for (size_t taskIndex = 0; taskIndex < numTasks; ++taskIndex)
        {
            auto taskData = function.PointerOffset(taskArgStorage, taskIndex);
            function.FillStruct(taskData, taskArgs[taskIndex]);
        }
    }

    void IRThreadPoolTaskArray::WaitAll(IRFunctionEmitter& function)
    {
        // Wait for all the tasks to finish
        _taskQueue.WaitAll(function);
    }

    IRThreadPoolTask IRThreadPoolTaskArray::GetTask(IRFunctionEmitter& function, size_t taskIndex)
    {
        return GetTask(function, function.Literal<int>(taskIndex));
    }

    IRThreadPoolTask IRThreadPoolTaskArray::GetTask(IRFunctionEmitter& function, llvm::Value* taskIndex)
    {
        assert(_taskArrayData != nullptr);

        auto& module = function.GetModule();
        auto& context = module.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto int8PtrPtrType = int8PtrType->getPointerTo();
        auto taskFunctionType = llvm::FunctionType::get(int8PtrType, { int8PtrType }, false);

        // Allocate some local variables (for conditional values)
        llvm::Value* taskFunctionVar = function.Variable(int8PtrType, "taskFunction");
        llvm::Value* taskDataVar = function.Variable(int8PtrType, "taskArgStorage");
        llvm::Value* taskReturnValueVar = function.Variable(int8PtrPtrType, "taskReturnValue");

        auto ife = function.If();
        ife.If(TypedComparison::greaterThanOrEquals, taskIndex, function.Literal<int>(0));
        {
            // Get pointers into struct fields
            auto returnValuesStoragePtr = GetReturnValuesStoragePointer(function);
            auto taskArgStoragePtr = GetTaskArgsStoragePointer(function);

            auto taskFunctionPtr = function.CastPointer(GetTaskFunctionPointer(function), taskFunctionType->getPointerTo()->getPointerTo());
            auto taskFunction = function.Load(taskFunctionPtr);
            auto taskArgStructSize = GetTaskArgsStructSize(function);
            auto taskReturnValuesStorage = function.Load(returnValuesStoragePtr);
            auto taskArgStorage = function.Load(taskArgStoragePtr);

            // Doing some pointer arithmetic here to account for not knowing the size of the argument struct
            auto taskData = function.PointerOffset(taskArgStorage, function.Operator(TypedOperator::multiply, taskIndex, taskArgStructSize));
            auto taskReturnValue = function.PointerOffset(taskReturnValuesStorage, taskIndex);

            function.Store(taskFunctionVar, function.CastPointer(taskFunction, int8PtrType));
            function.Store(taskDataVar, taskData);
            function.Store(taskReturnValueVar, taskReturnValue);
        }
        ife.Else(); // index < 0 -- return null task
        {
            function.Store(taskFunctionVar, function.NullPointer(int8PtrType));
            function.Store(taskDataVar, function.NullPointer(int8PtrType));
            function.Store(taskReturnValueVar, function.NullPointer(int8PtrPtrType));
        }
        ife.End();

        return { function.Load(taskFunctionVar), function.Load(taskDataVar), function.Load(taskReturnValueVar), this };
    }
}
}
