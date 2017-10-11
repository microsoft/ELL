////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PosixEmitterTest.cpp (emitters_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PosixEmitterTest.h"

// emitters
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRAsyncTask.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRExecutionEngine.h"
#include "IRFunctionEmitter.h"
#include "IRModuleEmitter.h"
#include "LLVMUtilities.h"

// testing
#include "testing.h"

// stl
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

using namespace ell;
using namespace ell::emitters;

using VoidReturningIntFunction = int (*)();

void TestPthreadSelf()
{
    // pthread_t pthread_self(void);
    IRModuleEmitter module("PthreadTest");
    module.DeclarePrintf();

    //
    // Regular function
    //
    std::string functionName = "TestSelf";
    auto func = module.BeginFunction(functionName, VariableType::Int32);
    auto selfVal = func.PthreadSelf();
    func.Printf("Self = %x\n", { selfVal });
    func.Return(selfVal);
    module.EndFunction();

    // print the generated code
    // module.DebugDump();

    IRExecutionEngine executionEngine(std::move(module));
    VoidReturningIntFunction compiledFunction = (VoidReturningIntFunction)executionEngine.ResolveFunctionAddress(functionName);

    auto self = compiledFunction();
    std::cout << "Got self = " << std::hex << self << std::dec << std::endl;
}

void TestPthreadCreate()
{
    // int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);

    IRModuleEmitter module("PthreadTest");
    module.DeclarePrintf();
    
    // Types
    auto& context = module.GetLLVMContext();
    auto pthreadType = module.GetRuntime().GetPosixEmitter().GetPthreadType();
    auto int8PtrType = llvm::Type::getInt8PtrTy(context);
    auto int8PtrPtrType = int8PtrType->getPointerTo();
    
    // Thread task function
    std::string taskFunctionName = "Task";
    auto taskFunction = module.BeginFunction(taskFunctionName, VariableType::Void, { VariableType::BytePointer });
    auto taskSelfVal = taskFunction.PthreadSelf();
    taskFunction.Printf("Task self = %x\n", { taskSelfVal });
    module.EndFunction();

    // Main function
    std::string mainFunctionName = "TestTask";
    auto mainFunction = module.BeginFunction(mainFunctionName, VariableType::Int32);
    auto selfVal = mainFunction.PthreadSelf();
    mainFunction.Printf("Main begin, self = %x\n", { selfVal });
    llvm::AllocaInst* threadVar1 = mainFunction.Variable(pthreadType, "thread1");
    llvm::AllocaInst* threadVar2 = mainFunction.Variable(pthreadType, "thread2");
    llvm::AllocaInst* threadVar3 = mainFunction.Variable(pthreadType, "thread3");
    llvm::AllocaInst* statusVar1 = mainFunction.Variable(int8PtrType, "status1");
    llvm::AllocaInst* statusVar2 = mainFunction.Variable(int8PtrType, "status2");
    llvm::AllocaInst* statusVar3 = mainFunction.Variable(int8PtrType, "status3");

    llvm::ConstantPointerNull* nullAttr = mainFunction.NullPointer(int8PtrType);
    llvm::ConstantPointerNull* nullArg = mainFunction.NullPointer(int8PtrPtrType);

    // Create some new threads
    auto errCode1 = mainFunction.PthreadCreate(threadVar1, nullAttr, taskFunction.GetFunction(), nullArg);
    auto errCode2 = mainFunction.PthreadCreate(threadVar2, nullAttr, taskFunction.GetFunction(), nullArg);
    auto errCode3 = mainFunction.PthreadCreate(threadVar3, nullAttr, taskFunction.GetFunction(), nullArg);

    // Wait for them to finish (one at a time)
    auto joinErrCode1 = mainFunction.PthreadJoin(mainFunction.Load(threadVar1), statusVar1);
    auto joinErrCode2 = mainFunction.PthreadJoin(mainFunction.Load(threadVar2), statusVar2);
    auto joinErrCode3 = mainFunction.PthreadJoin(mainFunction.Load(threadVar3), statusVar3);

    mainFunction.Print("Main end\n");
    mainFunction.Return(selfVal);
    module.EndFunction();

    // print the generated code
    module.DebugDump();

    IRExecutionEngine executionEngine(std::move(module));
    VoidReturningIntFunction compiledFunction = (VoidReturningIntFunction)executionEngine.ResolveFunctionAddress(mainFunctionName);

    auto self = compiledFunction();
}
