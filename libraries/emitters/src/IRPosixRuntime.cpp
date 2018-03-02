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

    llvm::Type* IRPosixRuntime::GetPointerSizedIntType()
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

    llvm::Type* IRPosixRuntime::GetTimespecPointerType()
    {
        return GetTimespecType()->getPointerTo();
    }

    //
    // pthreads -- types
    //

    llvm::Type* IRPosixRuntime::GetPthreadType()
    {
        auto& targetDevice = _module.GetCompilerOptions().targetDevice;
        if(targetDevice.IsLinux())
        {
            return GetPointerSizedIntType();
        }
        else if(targetDevice.IsMacOS())
        {
            return GetPointerSizedIntType();
        }
        else
        {
            return GetPointerSizedIntType();
        }
    }

    llvm::Type* IRPosixRuntime::GetPthreadMutexType()
    {
        auto& context = _module.GetLLVMContext();
        auto int8Type = llvm::Type::getInt8Ty(context);
        auto int32Type = llvm::Type::getInt32Ty(context);
        auto int64Type = llvm::Type::getInt64Ty(context);

        auto& targetDevice = _module.GetCompilerOptions().targetDevice;
        if(targetDevice.IsLinux())
        {
            auto triple = targetDevice.triple.empty() ? llvm::sys::getDefaultTargetTriple() : targetDevice.triple;

            if(triple.find("armv7") != std::string::npos)
            {
                // Raspbian (32-bit)
                // %union.pthread_mutex_t = type { %"struct.(anonymous union)::__pthread_mutex_s" }
                // %"struct.(anonymous union)::__pthread_mutex_s" = type { i32, i32, i32, i32, i32, %union.anon }
                // %union.anon = type { i32 }
                return _module.GetOrCreateStruct("pthread_mutex_t", {int32Type, int32Type, int32Type, int32Type, int32Type, int32Type});
            }
            else if(triple.find("aarch64") != std::string::npos)
            {
                // Linaro (64-bit)
                // %union.pthread_mutex_t = type { %"struct.(anonymous union)::__pthread_mutex_s", [8 x i8] }
                // %"struct.(anonymous union)::__pthread_mutex_s" = type { i32, i32, i32, i32, i32, i32, %struct.__pthread_internal_list }
                // %struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
                auto internalListType = _module.GetAnonymousStructType({GetPointerSizedIntType(), GetPointerSizedIntType()});
                return _module.GetOrCreateStruct("pthread_mutex_t", {int32Type, int32Type, int32Type, int32Type, int32Type, int32Type, internalListType});
            }
            else
            {
                assert(false && "Unknown Linux architecture");
                auto internalListType = _module.GetAnonymousStructType({GetPointerSizedIntType(), GetPointerSizedIntType()});
                return _module.GetOrCreateStruct("pthread_mutex_t", {int32Type, int32Type, int32Type, int32Type, int32Type, int32Type, internalListType});
            }
        }
        else if(targetDevice.IsMacOS())
        {
            // %struct._opaque_pthread_mutex_t = type { i64, [56 x i8] }
            return _module.GetOrCreateStruct("pthread_mutex_t", {int64Type, llvm::ArrayType::get(int8Type, 56)});
        }
        else
        {
            assert(false && "Unknown OS for pthreads");
            return GetPointerSizedIntType();
        }
    }

    llvm::Type* IRPosixRuntime::GetPthreadCondType()
    {
        auto& context = _module.GetLLVMContext();
        auto int8Type = llvm::Type::getInt8Ty(context);
        auto int8PtrType = int8Type->getPointerTo();
        auto int32Type = llvm::Type::getInt32Ty(context);
        auto int64Type = llvm::Type::getInt64Ty(context);

        auto& targetDevice = _module.GetCompilerOptions().targetDevice;
        if(targetDevice.IsLinux())
        {
            auto triple = targetDevice.triple.empty() ? llvm::sys::getDefaultTargetTriple() : targetDevice.triple;

            if(triple.find("armv7") != std::string::npos)
            {
                // Raspbian (32-bit)
                // %union.pthread_cond_t = type { %struct.anon }
                // %struct.anon = type { i32, i32, i64, i64, i64, i8*, i32, i32 }
                return _module.GetOrCreateStruct("pthread_cond_t", {int32Type, int32Type, int64Type, int64Type, int64Type, int8PtrType, int32Type, int32Type});
            }
            else if(triple.find("aarch64") != std::string::npos)
            {
                // Linaro (64-bit)
                // %union.pthread_cond_t = type { %struct.anon }
                // %struct.anon = type { i32, i32, i64, i64, i64, i8*, i32, i32 }
                return _module.GetOrCreateStruct("pthread_cond_t", {int32Type, int32Type, int64Type, int64Type, int64Type, int8PtrType, int32Type, int32Type});
            }
            else
            {
                assert(false && "Unknown Linux architecture");
                return _module.GetOrCreateStruct("pthread_cond_t", {int32Type, int32Type, int64Type, int64Type, int64Type, int8PtrType, int32Type, int32Type});
            }
        }
        else if(targetDevice.IsMacOS())
        {
            // %struct._opaque_pthread_cond_t = type { i64, [40 x i8] }
            return _module.GetOrCreateStruct("pthread_cond_t", {int64Type, llvm::ArrayType::get(int8Type, 40)});
        }
        else
        {
            assert(false && "Unknown OS for pthreads");
            return GetPointerSizedIntType();
        }
    }

    llvm::Type* IRPosixRuntime::GetPthreadSpinlockType()
    {
        auto& context = _module.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);

        auto& targetDevice = _module.GetCompilerOptions().targetDevice;
        if(targetDevice.IsLinux())
        {
            auto triple = targetDevice.triple.empty() ? llvm::sys::getDefaultTargetTriple() : targetDevice.triple;

            if(triple.find("armv7") != std::string::npos)
            {
                // Raspbian (32-bit)
                return int32Type;
            }
            else if(triple.find("aarch64") != std::string::npos)
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
        else if(targetDevice.IsMacOS())
        {
            assert(false && "pthread_spinlock not available on macOS target");
            return _module.GetAnonymousStructType({int32Type});
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
    llvm::Function* IRPosixRuntime::GetPthreadCreateFunction()
    {
        // Signature: int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);
        auto& context = _module.GetLLVMContext();
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
        auto intType = GetIntType();
        auto functionType = llvm::FunctionType::get(intType, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_getconcurrency", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadDetachFunction()
    {
        // Signature: int pthread_detach(pthread_t);
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
        auto pthreadType = GetPthreadType();
        auto functionType = llvm::FunctionType::get(pthreadType, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_self", functionType));
    }

    //
    // pthreads -- synchronization functions
    //
    llvm::Function* IRPosixRuntime::GetPthreadMutexInitFunction()
    {
        // Signature: int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutex_attr *attr);
        auto& context = _module.GetLLVMContext();
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        std::vector<llvm::Type*> args = { mutexPtrType, int8PtrType };
        auto functionType = llvm::FunctionType::get(intType, args, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_init", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadMutexDestroyFunction()
    {
        // Signature: int pthread_mutex_destroy(pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { mutexPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_destroy", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadMutexLockFunction()
    {
        // Signature: int pthread_mutex_lock(pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { mutexPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_lock", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadMutexTryLockFunction()
    {
        // Signature: int pthread_mutex_trylock(pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { mutexPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_trylock", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadMutexUnlockFunction()
    {
        // Signature: int pthread_mutex_unlock(pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { mutexPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_mutex_unlock", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadCondInitFunction()
    {
        // Signature: int pthread_cond_init(pthread_cond_t * cond, const pthread_cond_attr *attr);
        auto& context = _module.GetLLVMContext();
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto int8PtrType = llvm::Type::getInt8PtrTy(context);
        auto functionType = llvm::FunctionType::get(intType, { condPtrType, int8PtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_init", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadCondDestroyFunction()
    {
        // Signature: int pthread_cond_destroy(pthread_cond_t * cond);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_destroy", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadCondWaitFunction()
    {
        // Signature: int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType, mutexPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_wait", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadCondTimedwaitFunction()
    {
        // Signature: int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto mutexPtrType = GetPthreadMutexType()->getPointerTo();
        auto timespecPtrType = GetTimespecType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType, mutexPtrType, timespecPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_timedwait", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadCondSignalFunction()
    {
        // Signature: int pthread_cond_signal(pthread_cond_t * cond);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_signal", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadCondBroadcastFunction()
    {
        // Signature: int pthread_cond_broadcast(pthread_cond_t * cond);
        auto intType = GetIntType();
        auto condPtrType = GetPthreadCondType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { condPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_cond_broadcast", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadSpinInitFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_init(pthread_spinlock_t *lock, int pshared);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType, intType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_init", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadSpinLockFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_lock(pthread_spinlock_t *lock);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_lock", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadSpinTryLockFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_trylock(pthread_spinlock_t *lock);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_trylock", functionType));

    }

    llvm::Function* IRPosixRuntime::GetPthreadSpinUnlockFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_unlock(pthread_spinlock_t *lock);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_unlock", functionType));
    }

    llvm::Function* IRPosixRuntime::GetPthreadSpinDestroyFunction()
    {
        assert(_module.GetCompilerOptions().targetDevice.IsLinux() && "pthread spinlock only available on Linux");

        // Signature: int  pthread_spin_destroy(pthread_spinlock_t *lock);
        auto intType = GetIntType();
        auto spinlockPtrType = GetPthreadSpinlockType()->getPointerTo();
        auto functionType = llvm::FunctionType::get(intType, { spinlockPtrType }, false);
        return static_cast<llvm::Function*>(_module.GetLLVMModule()->getOrInsertFunction("pthread_spin_destroy", functionType));
    }


    // Signature: int pthread_once(pthread_once_t * once_init, void (*init_routine)(void)));
}
}
