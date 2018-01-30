////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRPosixRuntime.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"

// llvm
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

namespace ell
{
namespace emitters
{
    class IRModuleEmitter;
    class IRFunctionEmitter;
    class IRRuntime;

    //
    // POSIX threads API:
    //

    /// <summary> General functions and datatypes for POSIX routines. </summary>
    class IRPosixRuntime
    {
    public:
        //
        // time
        //

        /// <summary> Gets the LLVM type for the `timespec` structure on the current target. </summary>
        llvm::StructType* GetTimespecType();

        /// <summary> Gets the LLVM type for a pointer to the `timespec` structure on the current target. </summary>
        llvm::Type* GetTimespecPointerType();

        //
        // pthreads
        //

        /// <summary> Indicates if the pthreads API is available on the current target. </summary>
        bool IsPthreadsAvailable() const;

        //
        // pthreads -- thread functions
        //

        /// <summary> Gets an llvm::Function* representing the pthread_create function. </summary>
        /// int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);
        llvm::Function* GetPthreadCreateFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_equal function. </summary>
        /// int pthread_equal(pthread_t _Nullable, pthread_t _Nullable);
        llvm::Function* GetPthreadEqualFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_exit function. </summary>
        /// void pthread_exit(void* status);
        llvm::Function* GetPthreadExitFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_getconcurrency function. </summary>
        /// int pthread_getconcurrency(void);
        llvm::Function* GetPthreadGetConcurrencyFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_detach function. </summary>
        /// int pthread_detach(pthread_t);
        llvm::Function* GetPthreadDetachFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_join function. </summary>
        /// int pthread_join(pthread_t thread, void ** status);
        llvm::Function* GetPthreadJoinFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_self function. </summary>
        /// pthread_t pthread_self(void);
        llvm::Function* GetPthreadSelfFunction();

        // pthreads -- synchronization functions

        /// <summary> Gets an llvm::Function* representing the pthread_mutex_init function. </summary>
        // int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutex_attr *attr);
        llvm::Function* GetPthreadMutexInitFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_mutex_destroy function. </summary>
        /// int pthread_mutex_destroy(pthread_mutex_t * mutex);
        llvm::Function* GetPthreadMutexDestroyFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_mutex_lock function. </summary>
        /// int pthread_mutex_lock(pthread_mutex_t * mutex);
        llvm::Function* GetPthreadMutexLockFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_mutex_trylock function. </summary>
        /// int pthread_mutex_trylock(pthread_mutex_t * mutex);
        llvm::Function* GetPthreadMutexTryLockFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_mutex_unlock function. </summary>
        /// int pthread_mutex_unlock(pthread_mutex_t * mutex);
        llvm::Function* GetPthreadMutexUnlockFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_cond_init function. </summary>
        /// int pthread_cond_init(pthread_cond_t * cond, const pthread_cond_attr *attr);
        llvm::Function* GetPthreadCondInitFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_cond_destroy function. </summary>
        /// int pthread_cond_destroy(pthread_cond_t * cond);
        llvm::Function* GetPthreadCondDestroyFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_cond_wait function. </summary>
        /// int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex);
        llvm::Function* GetPthreadCondWaitFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_cond_timedwait function. </summary>
        /// int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime);
        llvm::Function* GetPthreadCondTimedwaitFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_cond_signal function. </summary>
        /// int pthread_cond_signal(pthread_cond_t * cond);
        llvm::Function* GetPthreadCondSignalFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_cond_broadcast function. </summary>
        /// int pthread_cond_broadcast(pthread_cond_t * cond);
        llvm::Function* GetPthreadCondBroadcastFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_spin_init function. </summary>
        /// int  pthread_spin_init(pthread_spinlock_t *lock, int pshared);
        llvm::Function* GetPthreadSpinInitFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_spin_lock function. </summary>
        /// int  pthread_spin_lock(pthread_spinlock_t *lock);
        llvm::Function* GetPthreadSpinLockFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_spin_trylock function. </summary>
        /// int  pthread_spin_trylock(pthread_spinlock_t *lock);
        llvm::Function* GetPthreadSpinTryLockFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_spin_unlock function. </summary>
        /// int  pthread_spin_unlock(pthread_spinlock_t *lock);
        llvm::Function* GetPthreadSpinUnlockFunction();

        /// <summary> Gets an llvm::Function* representing the pthread_spin_destroy function. </summary>
        /// int  pthread_spin_destroy(pthread_spinlock_t *lock);
        llvm::Function* GetPthreadSpinDestroyFunction();

        // pthreads -- types
        /// <summary> Gets the LLVM type for the `pthread_t` type on the current target. </summary>
        llvm::Type* GetPthreadType();

        /// <summary> Gets the LLVM type for the `pthread_mutex_t` type on the current target. </summary>
        llvm::Type* GetPthreadMutexType();

        /// <summary> Gets the LLVM type for the `pthread_cond_t` type on the current target. </summary>
        llvm::Type* GetPthreadCondType();

        /// <summary> Gets the LLVM type for the `pthread_spinlock_t` type on the current target. </summary>
        llvm::Type* GetPthreadSpinlockType();

        /// <summary> Gets the LLVM type for a pthread thread function ( `void* threadFn(void*)` )on the current target. </summary>
        llvm::FunctionType* GetPthreadStartRoutineType();

        // GetPthreadAttrType
        // GetPthreadOnceType

    private:
        friend IRModuleEmitter;
        friend IRRuntime;
        IRPosixRuntime(IRModuleEmitter& module);

        llvm::Type* GetIntType(); // returns LLVM type for native `int`
        llvm::Type* GetPointerSizedIntType(); // returns LLVM type for an int the size of a pointer

        IRModuleEmitter& _module;

        // Cached types
        llvm::StructType* _timespecType = nullptr;
    };
}
}
