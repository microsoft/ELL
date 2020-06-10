////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRPosixRuntime.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "LLVMUtilities.h"

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
        LLVMType GetTimespecPointerType();

        //
        // pthreads
        //

        /// <summary> Indicates if the pthreads API is available on the current target. </summary>
        bool IsPthreadsAvailable() const;

        //
        // pthreads -- thread functions
        //

        /// <summary> Gets an LLVMFunction representing the pthread_create function. </summary>
        /// int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);
        LLVMFunction GetPthreadCreateFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_equal function. </summary>
        /// int pthread_equal(pthread_t _Nullable, pthread_t _Nullable);
        LLVMFunction GetPthreadEqualFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_exit function. </summary>
        /// void pthread_exit(void* status);
        LLVMFunction GetPthreadExitFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_getconcurrency function. </summary>
        /// int pthread_getconcurrency(void);
        LLVMFunction GetPthreadGetConcurrencyFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_detach function. </summary>
        /// int pthread_detach(pthread_t);
        LLVMFunction GetPthreadDetachFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_join function. </summary>
        /// int pthread_join(pthread_t thread, void ** status);
        LLVMFunction GetPthreadJoinFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_self function. </summary>
        /// pthread_t pthread_self(void);
        LLVMFunction GetPthreadSelfFunction();

        // pthreads -- synchronization functions

        /// <summary> Gets an LLVMFunction representing the pthread_mutex_init function. </summary>
        // int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutex_attr *attr);
        LLVMFunction GetPthreadMutexInitFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_mutex_destroy function. </summary>
        /// int pthread_mutex_destroy(pthread_mutex_t * mutex);
        LLVMFunction GetPthreadMutexDestroyFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_mutex_lock function. </summary>
        /// int pthread_mutex_lock(pthread_mutex_t * mutex);
        LLVMFunction GetPthreadMutexLockFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_mutex_trylock function. </summary>
        /// int pthread_mutex_trylock(pthread_mutex_t * mutex);
        LLVMFunction GetPthreadMutexTryLockFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_mutex_unlock function. </summary>
        /// int pthread_mutex_unlock(pthread_mutex_t * mutex);
        LLVMFunction GetPthreadMutexUnlockFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_cond_init function. </summary>
        /// int pthread_cond_init(pthread_cond_t * cond, const pthread_cond_attr *attr);
        LLVMFunction GetPthreadCondInitFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_cond_destroy function. </summary>
        /// int pthread_cond_destroy(pthread_cond_t * cond);
        LLVMFunction GetPthreadCondDestroyFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_cond_wait function. </summary>
        /// int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex);
        LLVMFunction GetPthreadCondWaitFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_cond_timedwait function. </summary>
        /// int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime);
        LLVMFunction GetPthreadCondTimedwaitFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_cond_signal function. </summary>
        /// int pthread_cond_signal(pthread_cond_t * cond);
        LLVMFunction GetPthreadCondSignalFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_cond_broadcast function. </summary>
        /// int pthread_cond_broadcast(pthread_cond_t * cond);
        LLVMFunction GetPthreadCondBroadcastFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_spin_init function. </summary>
        /// int  pthread_spin_init(pthread_spinlock_t *lock, int pshared);
        LLVMFunction GetPthreadSpinInitFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_spin_lock function. </summary>
        /// int  pthread_spin_lock(pthread_spinlock_t *lock);
        LLVMFunction GetPthreadSpinLockFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_spin_trylock function. </summary>
        /// int  pthread_spin_trylock(pthread_spinlock_t *lock);
        LLVMFunction GetPthreadSpinTryLockFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_spin_unlock function. </summary>
        /// int  pthread_spin_unlock(pthread_spinlock_t *lock);
        LLVMFunction GetPthreadSpinUnlockFunction();

        /// <summary> Gets an LLVMFunction representing the pthread_spin_destroy function. </summary>
        /// int  pthread_spin_destroy(pthread_spinlock_t *lock);
        LLVMFunction GetPthreadSpinDestroyFunction();

        // pthreads -- types
        /// <summary> Gets the LLVM type for the `pthread_t` type on the current target. </summary>
        LLVMType GetPthreadType();

        /// <summary> Gets the LLVM type for the `pthread_mutex_t` type on the current target. </summary>
        LLVMType GetPthreadMutexType();

        /// <summary> Gets the LLVM type for the `pthread_cond_t` type on the current target. </summary>
        LLVMType GetPthreadCondType();

        /// <summary> Gets the LLVM type for the `pthread_spinlock_t` type on the current target. </summary>
        LLVMType GetPthreadSpinlockType();

        /// <summary> Gets the LLVM type for a pthread thread function ( `void* threadFn(void*)` )on the current target. </summary>
        llvm::FunctionType* GetPthreadStartRoutineType();

        // GetPthreadAttrType
        // GetPthreadOnceType

        IRPosixRuntime(const IRPosixRuntime&) = delete;

    private:
        friend IRModuleEmitter;
        friend IRRuntime;
        explicit IRPosixRuntime(IRModuleEmitter& module);

        LLVMType GetIntType(); // returns LLVM type for native `int`
        LLVMType GetPointerSizedIntType(); // returns LLVM type for an int the size of a pointer

        IRModuleEmitter& _module;

        // Cached types
        llvm::StructType* _timespecType = nullptr;
    };
} // namespace emitters
} // namespace ell
