////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRPosixRuntime.cpp (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRPosixRuntime.h"
#include "IRModuleEmitter.h"

// llvm
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

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

    LLVMType IRPosixRuntime::GetIntType()
    {
        // TODO: should this be 64 bits for 64-bit systems?
        auto& context = _module.GetLLVMContext();
        return llvm::Type::getInt32Ty(context);
    }

    LLVMType IRPosixRuntime::GetPointerSizedIntType()
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

        if (_module.GetCompilerOptions().targetDevice.numBits == 32)
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

    LLVMType IRPosixRuntime::GetTimespecPointerType()
    {
        return GetTimespecType()->getPointerTo();
    }

    //
    // pthreads -- types
    //

    LLVMType IRPosixRuntime::GetPthreadType()
    {
        auto& targetDevice = _module.GetCompilerOptions().targetDevice;
        if (targetDevice.IsLinux())
        {
            return GetPointerSizedIntType();
        }
        else if (targetDevice.IsMacOS())
        {
            return GetPointerSizedIntType();
        }
        else
        {
            return GetPointerSizedIntType();
        }
    }

    LLVMType IRPosixRuntime::GetPthreadMutexType()
    {
        auto& context = _module.GetLLVMContext();
        auto int8Type = llvm::Type::getInt8Ty(context);
        auto int16Type = llvm::Type::getInt16Ty(context);
        auto int32Type = llvm::Type::getInt32Ty(context);
        auto int64Type = llvm::Type::getInt64Ty(context);

        auto& targetDevice = _module.GetCompilerOptions().targetDevice;
        if (targetDevice.IsLinux())
        {
            auto triple = targetDevice.triple.empty() ? llvm::sys::getDefaultTargetTriple() : targetDevice.triple;

            if ((triple.find("armv6") != std::string::npos) || (triple.find("armv7") != std::string::npos))
            {
                // Raspbian (32-bit, pi0 or pi3)
                // %union.pthread_mutex_t = type { %"struct.(anonymous union)::__pthread_mutex_s" }
                // %"struct.(anonymous union)::__pthread_mutex_s" = type { i32, i32, i32, i32, i32, %union.anon }
                // %union.anon = type { i32 }
                return _module.GetOrCreateStruct("pthread_mutex_t", { int32Type, int32Type, int32Type, int32Type, int32Type, int32Type });
            }
            else if (triple.find("aarch64") != std::string::npos)
            {
                // Linaro (64-bit)
                // %union.pthread_mutex_t = type { %"struct.(anonymous union)::__pthread_mutex_s", [8 x i8] }
                // %"struct.(anonymous union)::__pthread_mutex_s" = type { i32, i32, i32, i32, i32, i32, %struct.__pthread_internal_list }
                // %struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
                auto internalListType = _module.GetAnonymousStructType({ GetPointerSizedIntType(), GetPointerSizedIntType() });
                return _module.GetOrCreateStruct("pthread_mutex_t", { int32Type, int32Type, int32Type, int32Type, int32Type, int32Type, internalListType });
            }
            else if (triple.find("x86_64") != std::string::npos)
            {
                // Linux 64-bit generic
                // %union.pthread_mutex_t = type { %"struct.(anonymous union)::__pthread_mutex_s" }
                // %"struct.(anonymous union)::__pthread_mutex_s" = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
                // %struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
                auto internalListType = _module.GetAnonymousStructType({GetPointerSizedIntType(), GetPointerSizedIntType()});
                return _module.GetOrCreateStruct("pthread_mutex_t", {int32Type, int32Type, int32Type, int32Type, int32Type, int16Type, int16Type, internalListType});
            }
            else
            {
                assert(false && "Unknown Linux architecture");
                auto internalListType = _module.GetAnonymousStructType({ GetPointerSizedIntType(), GetPointerSizedIntType() });
                return _module.GetOrCreateStruct("pthread_mutex_t", { int32Type, int32Type, int32Type, int32Type, int32Type, int32Type, internalListType });
            }
        }
        else if (targetDevice.IsMacOS())
        {
            // %struct._opaque_pthread_mutex_t = type { i64, [56 x i8] }
            return _module.GetOrCreateStruct("pthread_mutex_t", { int64Type, llvm::ArrayType::get(int8Type, 56) });
        }
        else
        {
            assert(false && "Unknown OS for pthreads");
            return GetPointerSizedIntType();
        }
    }

    LLVMType IRPosixRuntime::GetPthreadCondType()
    {
        auto& context = _module.GetLLVMContext();
        auto int8Type = llvm::Type::getInt8Ty(context);
        auto int8PtrType = int8Type->getPointerTo();
        auto int32Type = llvm::Type::getInt32Ty(context);
        auto int64Type = llvm::Type::getInt64Ty(context);

        auto& targetDevice = _module.GetCompilerOptions().targetDevice;
        if (targetDevice.IsLinux())
        {
            auto triple = targetDevice.triple.empty() ? llvm::sys::getDefaultTargetTriple() : targetDevice.triple;

            if ((triple.find("armv6") != std::string::npos) || (triple.find("armv7") != std::string::npos))
            {
                // Raspbian (32-bit, pi0 or pi3)
                // %union.pthread_cond_t = type { %struct.anon }
                // %struct.anon = type { i32, i32, i64, i64, i64, i8*, i32, i32 }
                return _module.GetOrCreateStruct("pthread_cond_t", { int32Type, int32Type, int64Type, int64Type, int64Type, int8PtrType, int32Type, int32Type });
            }
            else if (triple.find("aarch64") != std::string::npos)
            {
                // Linaro (64-bit)
                // %union.pthread_cond_t = type { %struct.anon }
                // %struct.anon = type { i32, i32, i64, i64, i64, i8*, i32, i32 }
                return _module.GetOrCreateStruct("pthread_cond_t", { int32Type, int32Type, int64Type, int64Type, int64Type, int8PtrType, int32Type, int32Type });
            }
            else if (triple.find("x86_64") != std::string::npos)
            {
                // Linux 64-bit generic
                // %union.pthread_cond_t = type { %struct.anon }
                // % struct.anon = type { i32, i32, i64, i64, i64, i8*, i32, i32 }
                return _module.GetOrCreateStruct("pthread_cond_t", {int32Type, int32Type, int64Type, int64Type, int64Type, int8PtrType, int32Type, int32Type});
            }
            else
            {
                assert(false && "Unknown Linux architecture");
                return _module.GetOrCreateStruct("pthread_cond_t", { int32Type, int32Type, int64Type, int64Type, int64Type, int8PtrType, int32Type, int32Type });
            }
        }
        else if (targetDevice.IsMacOS())
        {
            // %struct._opaque_pthread_cond_t = type { i64, [40 x i8] }
            return _module.GetOrCreateStruct("pthread_cond_t", { int64Type, llvm::ArrayType::get(int8Type, 40) });
        }
        else
        {
            assert(false && "Unknown OS for pthreads");
            return GetPointerSizedIntType();
        }
    }

    LLVMType IRPosixRuntime::GetPthreadSpinlockType()
    {
        auto& context = _module.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);

        auto& targetDevice = _module.GetCompilerOptions().targetDevice;
        if (targetDevice.IsLinux())
        {
            auto triple = targetDevice.triple.empty() ? llvm::sys::getDefaultTargetTriple() : targetDevice.triple;

            if ((triple.find("armv6") != std::string::npos) || (triple.find("armv7") != std::string::npos))
            {
                // Raspbian (32-bit, pi0 or pi3)
                return int32Type;
            }
            else if (triple.find("aarch64") != std::string::npos)
            {
                // Linaro (64-bit)
                return int32Type;
            }
            else
            {
                assert(false && "Unknown Linux architecture");
                return int32Type;
            }
        }
        else if (targetDevice.IsMacOS())
        {
            assert(false && "pthread_spinlock not available on macOS target");
            return _module.GetAnonymousStructType({ int32Type });
        }
        else
        {
            assert(false && "Unknown OS for pthreads");
            return GetPointerSizedIntType();
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

    //
    // pthreads -- thread functions
    //
    LLVMFunction IRPosixRuntime::GetPthreadCreateFunction()
    {
        // Signature: int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);
        auto& context = _module.GetLLVMContext();
        auto intType = GetIntType();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto pthreadPtrType = GetPthreadType()->getPointerTo();
        auto threadFunctionType = llvm::FunctionType::get(int8PtrType, { int8PtrType }, false);
        std::vector<LLVMType> args = { pthreadPtrType,
                                          int8PtrType,
                                          static_cast<LLVMType>(threadFunctionType)->getPointerTo(),
                                          int8PtrType };
        auto functionType = llvm::FunctionType::get(intType, args, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_create", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadEqualFunction()
    {
        // Signature: int pthread_equal(pthread_t t1, pthread_t t2);
        auto pthreadType = GetPthreadType();
        auto intType = GetIntType();
        auto functionType = llvm::FunctionType::get(intType, { pthreadType, pthreadType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_equal", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadExitFunction()
    {
        // Signature: void pthread_exit(void* status);
        auto& context = _module.GetLLVMContext();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto voidType = llvm::Type::getVoidTy(context);
        auto functionType = llvm::FunctionType::get(voidType, { int8PtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_exit", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadGetConcurrencyFunction()
    {
        // Signature: int pthread_getconcurrency(void);
        auto intType = GetIntType();
        auto functionType = llvm::FunctionType::get(intType, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_getconcurrency", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadDetachFunction()
    {
        // Signature: int pthread_detach(pthread_t);
        auto pthreadType = GetPthreadType();
        auto intType = GetIntType();
        auto functionType = llvm::FunctionType::get(intType, { pthreadType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_detach", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadJoinFunction()
    {
        // Signature: int pthread_join(pthread_t thread, void ** status);
        auto& context = _module.GetLLVMContext();
        auto pthreadType = GetPthreadType();
        auto intType = GetIntType();
        auto int8PtrPtrType = llvm::Type::getInt8PtrTy(context)->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { pthreadType, int8PtrPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_join", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadSelfFunction()
    {
        // Signature: pthread_t pthread_self(void);
        auto pthreadType = GetPthreadType();
        auto functionType = llvm::FunctionType::get(pthreadType, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_self", functionType));
    }

    //
    // pthreads -- synchronization functions
    //
    LLVMFunction IRPosixRuntime::GetPthreadMutexInitFunction()
    {
        // Signature: int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutex_attr *attr);
        auto& context = _module.GetLLVMContext();
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        std::vector<LLVMType> args = { mutexPtrType, int8PtrType };
        auto functionType = llvm::FunctionType::get(intType, args, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_init", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadMutexDestroyFunction()
    {
        // Signature: int pthread_mutex_destroy(pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { mutexPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_destroy", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadMutexLockFunction()
    {
        // Signature: int pthread_mutex_lock(pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { mutexPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_lock", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadMutexTryLockFunction()
    {
        // Signature: int pthread_mutex_trylock(pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { mutexPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_trylock", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadMutexUnlockFunction()
    {
        // Signature: int pthread_mutex_unlock(pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { mutexPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_unlock", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadCondInitFunction()
    {
        // Signature: int pthread_cond_init(pthread_cond_t * cond, const pthread_cond_attr *attr);
        auto& context = _module.GetLLVMContext();
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto functionType = llvm::FunctionType::get(intType, { condPtrType, int8PtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_init", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadCondDestroyFunction()
    {
        // Signature: int pthread_cond_destroy(pthread_cond_t * cond);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_destroy", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadCondWaitFunction()
    {
        // Signature: int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType, mutexPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_wait", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadCondTimedwaitFunction()
    {
        // Signature: int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto timespecPtrType = GetTimespecType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType, mutexPtrType, timespecPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_timedwait", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadCondSignalFunction()
    {
        // Signature: int pthread_cond_signal(pthread_cond_t * cond);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_signal", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadCondBroadcastFunction()
    {
        // Signature: int pthread_cond_broadcast(pthread_cond_t * cond);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_broadcast", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadSpinInitFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_init(pthread_spinlock_t *lock, int pshared);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType, intType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_init", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadSpinLockFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_lock(pthread_spinlock_t *lock);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_lock", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadSpinTryLockFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_trylock(pthread_spinlock_t *lock);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_trylock", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadSpinUnlockFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_unlock(pthread_spinlock_t *lock);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_unlock", functionType));
    }

    LLVMFunction IRPosixRuntime::GetPthreadSpinDestroyFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_destroy(pthread_spinlock_t *lock);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType }, false);
        return static_cast<LLVMFunction>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_destroy", functionType));
    }

    // Signature: int pthread_once(pthread_once_t * once_init, void (*init_routine)(void)));
}
}
