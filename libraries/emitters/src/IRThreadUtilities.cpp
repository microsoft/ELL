////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRThreadUtilities.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRThreadUtilities.h"
#include "IRFunctionEmitter.h"

namespace ell
{
namespace emitters
{
    //
    // Functions
    //

    llvm::StructType* GetTaskArgStructType(IRModuleEmitter& module, llvm::Function* taskFunction)
    {
        std::vector<llvm::Type*> argTypes;
        std::transform(taskFunction->arg_begin(), taskFunction->arg_end(), std::back_inserter(argTypes), [](auto& arg) { return arg.getType(); });
        return module.GetAnonymousStructType(argTypes);
    }

    llvm::Function* GetTaskWrapperFunction(IRModuleEmitter& module, llvm::Function* taskFunction)
    {
        auto& context = module.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);

        auto taskFunctionName = taskFunction->getGlobalIdentifier();
        auto wrapperFunctionName = std::string("wrap_") + taskFunctionName;

        if (module.HasFunction(wrapperFunctionName))
        {
            return module.GetFunction(wrapperFunctionName);
        }

        auto taskArgType = GetTaskArgStructType(module, taskFunction);

        auto taskWrapperFunction = module.BeginFunction(wrapperFunctionName, int8PtrType, { int8PtrType });
        {
            auto arguments = taskWrapperFunction.Arguments().begin();
            auto threadArg = &(*arguments);
            auto argStructPtr = taskWrapperFunction.CastPointer(threadArg, taskArgType->getPointerTo());

            // Should be a 1:1 correspondence between args and fields in the struct
            auto numFields = taskArgType->getNumElements();
            std::vector<llvm::Value*> taskFunctionArgs;

            for (size_t fieldIndex = 0; fieldIndex < numFields; ++fieldIndex)
            {
                auto fieldPtr = taskWrapperFunction.GetStructFieldPointer(argStructPtr, fieldIndex);
                taskFunctionArgs.push_back(taskWrapperFunction.Load(fieldPtr));
            }

            auto functionResult = taskWrapperFunction.Call(taskFunction, taskFunctionArgs);
            if (functionResult->getType()->isSized())
            {
                auto resultCast = taskWrapperFunction.BitCast(functionResult, int8PtrType);
                taskWrapperFunction.Return(resultCast);
            }
            else
            {
                taskWrapperFunction.Return(taskWrapperFunction.NullPointer(int8PtrType));
            }
        }
        module.EndFunction();
        return taskWrapperFunction.GetFunction();
    }

    llvm::Function* GetTaskWrapperFunction(IRModuleEmitter& module, IRFunctionEmitter& taskFunction)
    {
        return GetTaskWrapperFunction(module, taskFunction.GetFunction());
    }
}
}
