////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRAsyncTask.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRAsyncTask.h"
#include "IRFunctionEmitter.h"
#include "IRHeaderWriter.h"
#include "IRModuleEmitter.h"
#include "IRThreadUtilities.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>

namespace ell
{
namespace emitters
{
    IRAsyncTask::IRAsyncTask(IRFunctionEmitter& owningFunction, llvm::Function* taskFunction, const std::vector<llvm::Value*>& arguments)
        : _taskFunction(taskFunction), _arguments(arguments)
    {
        const auto& compilerParameters = owningFunction.GetModule().GetCompilerParameters();
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
        _returnValuePtr = function.Variable(int8PtrType, "returnValue");
        _returnType = _taskFunction->getReturnType();

        // call function
        if (UsePthreads())
        {
            auto pthreadType = module.GetRuntime().GetPosixEmitter().GetPthreadType();
            auto taskArgType = GetTaskArgStructType(module, _taskFunction);
            llvm::ConstantPointerNull* nullAttr = function.NullPointer(int8PtrType);

            // Create stack variables for thread, argument struct, and output status
            auto taskArg = function.Variable(taskArgType, "taskArg");
            _pthread = function.Variable(pthreadType, "thread");
            function.FillStruct(taskArg, _arguments);
            auto pthreadWrapperFunction = GetTaskWrapperFunction(module, _taskFunction);
            auto errCode = function.PthreadCreate(_pthread, nullAttr, pthreadWrapperFunction, function.CastPointer(taskArg, int8PtrType));
        }
        else
        {
            function.Store(_returnValuePtr, function.Call(_taskFunction, _arguments));
        }
    }

    void IRAsyncTask::Wait(IRFunctionEmitter& functionEmitter)
    {
        if (UsePthreads())
        {
            auto errCode = functionEmitter.PthreadJoin(functionEmitter.Load(_pthread), _returnValuePtr);
            _returnValue = functionEmitter.Load(_returnValuePtr);
            auto castReturnValue = functionEmitter.BitCast(_returnValue, _returnType);
        }
        else
        {
            Run(functionEmitter);
            _returnValue = functionEmitter.Load(_returnValuePtr);
        }
    }

    llvm::Value* IRAsyncTask::GetReturnValue(IRFunctionEmitter& functionEmitter) const
    {
        if(_returnType != nullptr)
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
