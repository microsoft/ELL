////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRTask.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRTask.h"
#include "IRFunctionEmitter.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace emitters
{

    //
    // IRTask
    //

    IRTask::IRTask(const IRAsyncTask& asyncTask)
        : _type(TaskType::async), _task(asyncTask)
    {
    }

    IRTask::IRTask(const IRThreadPoolTask& threadPoolTask)
        : _type(TaskType::threadPool), _task(threadPoolTask)
    {
    }

    IRTask::~IRTask()
    {
        switch (_type)
        {
        case TaskType::async:
            _task.asyncTask.~IRAsyncTask();
            break;
        case TaskType::threadPool:
            _task.threadPoolTask.~IRThreadPoolTask();
            break;
        default:
            // not throwing since this is a dtor
            assert(false && "Error: illegal value for TaskType");
        }
    }

    void IRTask::Wait(IRFunctionEmitter& function)
    {
        switch (_type)
        {
        case TaskType::async:
            _task.asyncTask.Wait(function);
            break;
        case TaskType::threadPool:
            _task.threadPoolTask.Wait(function);
            break;
        default:
            throw std::invalid_argument("illegal value for TaskType");
        }
    }

    llvm::Value* IRTask::GetReturnValue(IRFunctionEmitter& function)
    {
        switch (_type)
        {
        case TaskType::async:
            return _task.asyncTask.GetReturnValue(function);
            break;
        case TaskType::threadPool:
            return _task.threadPoolTask.GetReturnValue(function);
            break;
        default:
            throw std::invalid_argument("illegal value for TaskType");
        }
    }

    llvm::Value* IRTask::IsNull(IRFunctionEmitter& function)
    {
        switch (_type)
        {
        case TaskType::async:
            return _task.asyncTask.IsNull(function);
        case TaskType::threadPool:
            return _task.threadPoolTask.IsNull(function);
        default:
            throw std::invalid_argument("illegal value for TaskType");
        }
    }

    //
    // IRTaskArray
    //

    IRTaskArray::IRTaskArray(const std::vector<IRAsyncTask>& asyncTasks)
        : _type(IRTask::TaskType::async), _tasks(asyncTasks)
    {
    }

    IRTaskArray::IRTaskArray(const IRThreadPoolTaskArray& threadPoolTasks)
        : _type(IRTask::TaskType::threadPool), _tasks(threadPoolTasks)
    {
    }

    IRTaskArray::~IRTaskArray()
    {
        if (_type == IRTask::TaskType::async)
        {
            switch (_type)
            {
            case IRTask::TaskType::async:
                using AsyncTaskVector = std::vector<IRAsyncTask>;
                _tasks.asyncTasks.~AsyncTaskVector();
                break;
            case IRTask::TaskType::threadPool:
                _tasks.threadPoolTasks.~IRThreadPoolTaskArray();
                break;
            default:
                // not throwing since this is a dtor
                assert(false && "Error: illegal value for TaskType");
            }
        }
    }

    void IRTaskArray::WaitAll(IRFunctionEmitter& function)
    {
        switch (_type)
        {
        case IRTask::TaskType::async:
            for (auto& t : _tasks.asyncTasks)
            {
                t.Wait(function);
            }
            break;
        case IRTask::TaskType::threadPool:
            _tasks.threadPoolTasks.WaitAll(function);
            break;
        default:
            throw std::invalid_argument("illegal value for TaskType");
        }
    }

    IRTask IRTaskArray::GetTask(IRFunctionEmitter& function, size_t taskIndex)
    {
        switch (_type)
        {
        case IRTask::TaskType::async:
            return _tasks.asyncTasks[taskIndex];
            break;
        case IRTask::TaskType::threadPool:
            return _tasks.threadPoolTasks.GetTask(function, function.Literal<int>(taskIndex));
            break;
        default:
            throw std::invalid_argument("illegal value for TaskType");
        }
    }
}
}
