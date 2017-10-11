////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRPosix.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "LLVMInclude.h"

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

    // // Basic routines
    // int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);
    // int pthread_equal(pthread_t _Nullable, pthread_t _Nullable);
    // void pthread_exit(void* status);
    // int pthread_getconcurrency(void);
    // int pthread_detach(pthread_t);
    // int pthread_join(pthread_t thread, void ** status);
    // pthread_t pthread_self(void);

    // // Synchronization
    // int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutex_attr *attr);
    // int pthread_mutex_destroy(pthread_mutex_t * mutex);
    // int pthread_mutex_lock(pthread_mutex_t * mutex);
    // int pthread_mutex_trylock(pthread_mutex_t * mutex);
    // int pthread_mutex_unlock(pthread_mutex_t * mutex);
    // int pthread_cond_init(pthread_cond_t * cond, const pthread_cond_attr *attr);
    // int pthread_cond_destroy(pthread_cond_t * cond);
    // int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex);
    // int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime);
    // int pthread_cond_signal(pthread_cond_t * cond);
    // int pthread_cond_broadcast(pthread_cond_t * cond);

    // int pthread_once(pthread_once_t * once_init, void (*init_routine)(void)));
 
    // // Thread-specific data routines
    // int pthread_key_create(pthread_key_t * key, void (* dest_routine(void *)));
    // int pthread_key_delete(pthread_key_t key);
    // int pthread_setspecific(pthread_key_t key, const void * pointer);
    // void * pthread_getspecific(pthread_key_t key);
    // void pthread_cleanup_push(void (*routine)(void *), void *routine_arg);
    // void pthread_cleanup_pop(int execute);

    // // Thread attribute routines
    // int pthread_attr_init(pthread_attr_t *attr);
    // int pthread_attr_destroy(pthread_attr_t *attr);
    // int pthread_attr_getstacksize(pthread_attr_t *attr, size_t * stacksize);
    // int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
    // int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *state);
    // int pthread_attr_setdetachstate(pthread_attr_t *attr, int state);

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

        // pthreads functions
        /// <summary> Gets an llvm::Function* representing the pthread_create function. </summary>
        llvm::Function* GetPthreadCreateFunction();
        
        /// <summary> Gets an llvm::Function* representing the pthread_equal function. </summary>
        llvm::Function* GetPthreadEqualFunction();
        
        /// <summary> Gets an llvm::Function* representing the pthread_exit function. </summary>
        llvm::Function* GetPthreadExitFunction();
        
        /// <summary> Gets an llvm::Function* representing the pthread_getconcurrency function. </summary>
        llvm::Function* GetPthreadGetConcurrencyFunction();
        
        /// <summary> Gets an llvm::Function* representing the pthread_detach function. </summary>
        llvm::Function* GetPthreadDetachFunction();
        
        /// <summary> Gets an llvm::Function* representing the pthread_join function. </summary>
        llvm::Function* GetPthreadJoinFunction();
        
        /// <summary> Gets an llvm::Function* representing the pthread_self function. </summary>
        llvm::Function* GetPthreadSelfFunction();

        // TODO:
        // pthread_mutex_init
        // pthread_mutex_destroy
        // pthread_mutex_lock
        // pthread_mutex_trylock
        // pthread_mutex_unlock
        // pthread_cond_init
        // pthread_cond_destroy
        // phtread_cond_wait
        // pthread_cond_timedwait
        // pthread_code_signal
        // pthread_cond_broadcast
        // pthread_once

        // pthreads types
        /// <summary> Gets the LLVM type for the `pthread_t` type on the current target. </summary>
        llvm::Type* GetPthreadType();

        /// <summary> Gets the LLVM type for a pthread thread function ( `void* threadFn(void*)` )on the current target. </summary>
        llvm::FunctionType* GetPthreadStartRoutineType();

        // TODO:
        // GetPthreadAttrType
        // GetPthreadMutexType
        // GetPthreadCondType
        // GetPthreadOnceType

    private:
        friend IRModuleEmitter;
        friend IRRuntime;
        IRPosixRuntime(IRModuleEmitter& module);

        llvm::Type* GetIntType(); // returns LLVM type for native `int`

        IRModuleEmitter& _module;

        // Cached types
        llvm::StructType* _timespecType = nullptr;
    };
}
}
