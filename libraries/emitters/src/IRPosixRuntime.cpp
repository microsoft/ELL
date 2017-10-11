////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRPosixRuntime.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRPosixRuntime.h"
#include "IRModuleEmitter.h"

// Helpful discussion on emitter pthread routines:
// https://stackoverflow.com/questions/19803848/llvm-insert-pthread-function-calls-into-ir

namespace ell
{
namespace emitters
{
    IRPosixRuntime::IRPosixRuntime(IRModuleEmitter& module)
        : _module(module)
    {
    }

    llvm::Type* IRPosixRuntime::GetIntType()
    {
        // TODO: should this be 64 bits for 64-bit systems?
        auto& context = _module.GetLLVMContext();
        return llvm::Type::getInt32Ty(context);
    }

    //
    // time
    //
    llvm::StructType* IRPosixRuntime::GetTimespecType()
    {
        if (_timespecType != nullptr)
        {
            return _timespecType;
        }

        auto& context = _module.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);
        auto int64Type = llvm::Type::getInt64Ty(context);

        if (_module.GetCompilerParameters().targetDevice.numBits == 32)
        {
            // These are really time_t and long
            _timespecType = llvm::StructType::create(context, { int32Type, int32Type }, "timespec");
        }
        else
        {
            _timespecType = llvm::StructType::create(context, { int64Type, int64Type }, "timespec");
        }
        return _timespecType;
    }

    llvm::Type* IRPosixRuntime::GetTimespecPointerType()
    {
        return GetTimespecType()->getPointerTo();
    }

    //
    // pthreads
    //
    llvm::Type* IRPosixRuntime::GetPthreadType()
    {
        auto& context = _module.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);
        auto int64Type = llvm::Type::getInt64Ty(context);
        auto pointerSize = _module.GetTargetDataLayout().getPointerSizeInBits();

        if (pointerSize == 32)
        {
            return int32Type;
        }
        else
        {
            return int64Type;
        }
    }

    llvm::FunctionType* IRPosixRuntime::GetPthreadStartRoutineType()
    {
        auto& context = _module.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);

        llvm::FunctionType* voidFunctionType = llvm::FunctionType::get(voidType, { int8PtrType }, false); // void(void *)
        return voidFunctionType;
    }

    llvm::Function* IRPosixRuntime::GetPthreadCreateFunction()
    {
        // Signature: int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);
        auto& context = _module.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);
        auto intType = GetIntType();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto pthreadPtrType = GetPthreadType()->getPointerTo();
        auto threadFunctionType = llvm::FunctionType::get(int8PtrType, { int8PtrType }, false);
        std::vector<llvm::Type*> args = { pthreadPtrType,
                                          int8PtrType,
                                          static_cast<llvm::Type*>(threadFunctionType)->getPointerTo(),
                                          int8PtrType };
        auto functionType = llvm::FunctionType::get(intType, args, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_create", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadEqualFunction()
    {
        // Signature: int pthread_equal(pthread_t t1, pthread_t t2);
        auto& context = _module.GetLLVMContext();
        auto pthreadType = GetPthreadType();
        auto intType = GetIntType();
        auto functionType = llvm::FunctionType::get(intType, { pthreadType, pthreadType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_equal", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadExitFunction()
    {
        // Signature: void pthread_exit(void* status);
        auto& context = _module.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto voidType = llvm::Type::getVoidTy(context);
        auto functionType = llvm::FunctionType::get(voidType, { int8PtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_exit", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadGetConcurrencyFunction()
    {
        // Signature: int pthread_getconcurrency(void);
        auto& context = _module.GetLLVMContext();
        auto pthreadType = GetPthreadType();
        auto intType = GetIntType();
        auto functionType = llvm::FunctionType::get(intType, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_getconcurrency", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadDetachFunction()
    {
        // Signature: int pthread_detach(pthread_t);
        auto& context = _module.GetLLVMContext();
        auto pthreadType = GetPthreadType();
        auto intType = GetIntType();
        auto functionType = llvm::FunctionType::get(intType, { pthreadType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_detach", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadJoinFunction()
    {
        // Signature: int pthread_join(pthread_t thread, void ** status);
        auto& context = _module.GetLLVMContext();
        auto pthreadType = GetPthreadType();
        auto intType = GetIntType();
        auto int8PtrPtrType = llvm::Type::getInt8PtrTy(context)->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { pthreadType, int8PtrPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_join", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadSelfFunction()
    {
        // Signature: pthread_t pthread_self(void);
        auto& context = _module.GetLLVMContext();
        auto pthreadType = GetPthreadType();
        auto intType = GetIntType();
        auto functionType = llvm::FunctionType::get(pthreadType, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_self", functionType));
    }
}
}
