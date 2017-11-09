////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRAsyncTask.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRAsyncTask.h"
#include "IRFunctionEmitter.h"
#include "IRModuleEmitter.h"

#include "IRHeaderWriter.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>

namespace ell
{
namespace emitters
{
    IRAsyncTask::IRAsyncTask(IRFunctionEmitter& owningFunction, llvm::Function* taskFunction, const std::vector<llvm::Value*>& arguments)
        : _owningFunction(owningFunction), _taskFunction(taskFunction), _arguments(arguments)
    {
        _usePthreads = owningFunction.GetModule().GetCompilerParameters().parallelize;
        if (UsePthreads())
        {
            std::cout << "Starting in async mode" << std::endl;
            StartTask();
        }
    }

    IRAsyncTask::IRAsyncTask(IRFunctionEmitter& owningFunction, IRFunctionEmitter& taskFunction, const std::vector<llvm::Value*>& arguments)
        : IRAsyncTask(owningFunction, taskFunction.GetFunction(), arguments)
    {
    }

    void IRAsyncTask::Run()
    {
        StartTask();
    }

    void IRAsyncTask::StartTask()
    {
        if (_started)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Task already started");
        }
        _started = true;

        // call function
        if (UsePthreads())
        {
            auto& module = _owningFunction.GetModule();
            auto& context = _owningFunction.GetLLVMContext();
            auto int8PtrType = llvm::Type::getInt8PtrTy(context);
            auto pthreadType = module.GetRuntime().GetPosixEmitter().GetPthreadType();
            auto taskArgType = GetTaskArgStructType();
            llvm::ConstantPointerNull* nullAttr = _owningFunction.NullPointer(int8PtrType);

            // Create stack variables for thread, argument struct, and output status
            auto taskArg = _owningFunction.Variable(taskArgType, "taskArg");
            _pthread = _owningFunction.Variable(pthreadType, "thread");
            _returnValuePtr = _owningFunction.Variable(int8PtrType, "status");
            _returnType = _taskFunction->getReturnType();
            
            _owningFunction.FillStruct(taskArg, _arguments);
            auto pthreadWrapperFunction = GetPthreadWrapper(taskArgType);
            auto errCode = _owningFunction.PthreadCreate(_pthread, nullAttr, pthreadWrapperFunction, _owningFunction.BitCast(taskArg, int8PtrType));
        }
        else
        {
            std::cout << "Starting in deferred mode" << std::endl;
            _returnValue = _owningFunction.Call(_taskFunction, _arguments);
        }
    }

    llvm::Function* IRAsyncTask::GetPthreadWrapper(llvm::StructType* taskArgType)
    {
        auto& module = _owningFunction.GetModule();
        auto& context = _owningFunction.GetLLVMContext();
        auto& emitter = _owningFunction.GetEmitter();
        auto& irBuilder = emitter.GetIRBuilder();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        llvm::ConstantPointerNull* nullPtr = _owningFunction.NullPointer(int8PtrType);

        auto taskFunctionName = _taskFunction->getGlobalIdentifier();
        auto wrapperFunctionName = std::string("wrap_") + taskFunctionName;

        if (module.HasFunction(wrapperFunctionName))
        {
            return module.GetFunction(wrapperFunctionName);
        }

        auto pthreadFunction = _owningFunction.GetModule().BeginFunction(wrapperFunctionName, emitters::VariableType::BytePointer, { emitters::VariableType::BytePointer });
        {
            auto& threadEmitter = pthreadFunction.GetEmitter();
            auto& threadIrBuilder = threadEmitter.GetIRBuilder();
            auto arguments = pthreadFunction.Arguments().begin();
            auto threadArg = &(*arguments);
            auto argStructPtr = pthreadFunction.CastPointer(threadArg, taskArgType->getPointerTo());

            // Should be a 1:1 correspondence between args and fields in the struct
            auto numFields = taskArgType->getNumElements();
            std::vector<llvm::Value*> taskFunctionArgs;

            for (int fieldIndex = 0; fieldIndex < numFields; ++fieldIndex)
            {
                auto fieldPtr = threadIrBuilder.CreateInBoundsGEP(argStructPtr, { pthreadFunction.Literal(0), pthreadFunction.Literal(fieldIndex) });
                taskFunctionArgs.push_back(pthreadFunction.Load(fieldPtr));
            }

            auto returnValue = pthreadFunction.Call(_taskFunction, taskFunctionArgs);
            pthreadFunction.Return(pthreadFunction.BitCast(returnValue, int8PtrType));
        }
        _owningFunction.GetModule().EndFunction();
        return pthreadFunction.GetFunction();
    }

    void IRAsyncTask::Sync()
    {
        if (UsePthreads())
        {
            auto errCode = _owningFunction.PthreadJoin(_owningFunction.Load(_pthread), _returnValuePtr);
            auto rawReturnValue = _owningFunction.Load(_returnValuePtr);
            _returnValue = _owningFunction.BitCast(rawReturnValue, _returnType);
        }
        else
        {
            StartTask();
        }
    }

    llvm::Value* IRAsyncTask::GetReturnValue() const
    {
        if (UsePthreads())
        {
            return _returnValue;
        }
        else
        {
            return _returnValue;
        }
    }

    llvm::StructType* IRAsyncTask::GetTaskArgStructType()
    {
        std::string taskFunctionName = _taskFunction->getGlobalIdentifier();
        auto& module = _owningFunction.GetModule();
        std::vector<llvm::Type*> argTypes;
        for (auto& argument : _taskFunction->args())
        {
            argTypes.push_back(argument.getType());
        }
        return module.GetAnonymousStructType(argTypes);
    }
}
}