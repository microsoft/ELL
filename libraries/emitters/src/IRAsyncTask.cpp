////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRAsyncTask.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRAsyncTask.h"
#include "IRFunctionEmitter.h"
#include "IRThreadUtilities.h"

// utilities
#include "Exception.h"
#include "Unused.h"

namespace ell
{
namespace emitters
{
    IRAsyncTask::IRAsyncTask(IRFunctionEmitter& owningFunction, llvm::Function* taskFunction, const std::vector<llvm::Value*>& arguments)
        : _taskFunction(taskFunction), _arguments(arguments)
    {
        const auto& compilerParameters = owningFunction.GetModule().GetCompilerOptions();
        _usePthreads = compilerParameters.parallelize && !compilerParameters.targetDevice.IsWindows();
        if (UsePthreads())
        {
            Run(owningFunction);
        }
    }

    IRAsyncTask::IRAsyncTask(IRFunctionEmitter& owningFunction, IRFunctionEmitter& taskFunction, const std::vector<llvm::Value*>& arguments)
        : IRAsyncTask(owningFunction, taskFunction.GetFunction(), arguments)
    {
    }

    void IRAsyncTask::Run(IRFunctionEmitter& function)
    {
        auto& module = function.GetModule();
        auto& context = function.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);

        // Allocate a stack variable for the return value
        _returnType = _taskFunction->getReturnType();

        // call function
        if (UsePthreads())
        {
            auto pthreadType = module.GetRuntime().GetPosixEmitter().GetPthreadType();
            auto taskArgType = GetTaskArgStructType(module, _taskFunction);
            llvm::ConstantPointerNull* nullAttr = function.NullPointer(int8PtrType);

            // Create stack variables for thread and argument struct
            _pthread = function.Variable(pthreadType, "thread");
            auto taskArg = function.Variable(taskArgType, "taskArg");
            function.FillStruct(taskArg, _arguments);
            auto pthreadWrapperFunction = GetTaskWrapperFunction(module, _taskFunction);
            auto errCode = function.PthreadCreate(_pthread, nullAttr, pthreadWrapperFunction, function.CastPointer(taskArg, int8PtrType));
            UNUSED(errCode);
        }
        else
        {
            _returnValue = function.Call(_taskFunction, _arguments);
        }
    }

    void IRAsyncTask::Wait(IRFunctionEmitter& functionEmitter)
    {
        if (UsePthreads())
        {
            auto& context = functionEmitter.GetLLVMContext();
            auto int8PtrType = llvm::Type::getInt8PtrTy(context);
            auto returnValuePtr = functionEmitter.Variable(int8PtrType, "returnValue");
            auto errCode = functionEmitter.PthreadJoin(functionEmitter.Load(_pthread), returnValuePtr);
            UNUSED(errCode);
            _returnValue = functionEmitter.Load(returnValuePtr);
        }
        else
        {
            Run(functionEmitter);
        }
    }

    llvm::Value* IRAsyncTask::GetReturnValue(IRFunctionEmitter& functionEmitter) const
    {
        if (_returnType != nullptr)
        {
            return functionEmitter.BitCast(_returnValue, _returnType);
        }
        else
        {
            return _returnValue;
        }
    }

    llvm::Value* IRAsyncTask::IsNull(IRFunctionEmitter& functionEmitter)
    {
        return functionEmitter.FalseBit();
    }

    //
    // Utility function
    //
    void SyncAllTasks(IRFunctionEmitter& function, std::vector<IRAsyncTask>& tasks)
    {
        for (auto& task : tasks)
        {
            task.Wait(function);
        }
    }
}
}
