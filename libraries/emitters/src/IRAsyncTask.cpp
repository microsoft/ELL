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
    static constexpr bool usePthreads = true; // For debugging: set to false to make calls synchronous

    IRAsyncTask::IRAsyncTask(IRFunctionEmitter& owningFunction, llvm::Function* taskFunction, const std::vector<llvm::Value*>& arguments)
        : _owningFunction(owningFunction), _taskFunction(taskFunction), _arguments(arguments), _returnValue(nullptr), _started(false), _pthread(nullptr), _statusVar(nullptr)
    {
        if (usePthreads)
        {
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
        if (usePthreads)
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
            _statusVar = _owningFunction.Variable(int8PtrType, "status");

            _owningFunction.FillStruct(taskArg, _arguments);
            auto pthreadWrapperFunction = GetPthreadWrapper(taskArgType);
            auto errCode = _owningFunction.PthreadCreate(_pthread, nullAttr, pthreadWrapperFunction, _owningFunction.Cast(taskArg, int8PtrType));
        }
        else
        {
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

            pthreadFunction.Call(_taskFunction, taskFunctionArgs);
            pthreadFunction.Return(nullPtr);
        }
        _owningFunction.GetModule().EndFunction();
        return pthreadFunction.GetFunction();
    }

    void IRAsyncTask::Sync()
    {
        if (usePthreads)
        {
            auto errCode = _owningFunction.PthreadJoin(_owningFunction.Load(_pthread), _statusVar);
            // TODO: get return value from statusVar
        }
        else
        {
            StartTask();
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
        return module.DeclareAnonymousStruct(argTypes);
    }
}
}