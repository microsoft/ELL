////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AsyncEmitterTest.cpp (emitters_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AsyncEmitterTest.h"

// emitters
#include "CompilerOptions.h"
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
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <thread>

using namespace ell;
using namespace ell::emitters;

//
// Helpers
//

using IntFunction = int (*)();
using UnaryScalarFloatFunction = float (*)(float);

//
// Tests
//
void TestIRAsyncTask(bool parallel)
{
    std::cout << "\nTesting IRAsyncTask Function in " << (parallel ? "parallel" : "serial") << " mode" << std::endl;
    CompilerOptions options;
    options.optimize = false;
    options.parallelize = parallel;
    options.targetDevice.deviceName = "host";
    IRModuleEmitter module("IRAsyncTaskTest", options);
    module.DeclarePrintf();

    // Common stuff for both functions
    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Float });

    //
    // Regular function
    //
    std::string syncFunctionName = "TestSync";
    auto syncFunction = module.BeginFunction(syncFunctionName, VariableType::Float, args);
    {
        llvm::Value* syncArg = syncFunction.GetFunctionArgument("x");
        llvm::Value* syncSum = syncFunction.Operator(emitters::GetAddForValueType<float>(), syncArg, syncFunction.Literal<float>(5.0));
        syncFunction.Return(syncSum);
    }
    module.EndFunction();

    //
    // Task function
    //
    auto taskFunction = module.BeginFunction("asyncTaskFunction", VariableType::Float, args);
    {
        auto arguments = taskFunction.Arguments().begin();
        auto argVal = &(*arguments++);
        llvm::Value* asyncSum = taskFunction.Operator(emitters::GetAddForValueType<float>(), argVal, taskFunction.Literal<float>(5.0));
        taskFunction.Return(asyncSum);
    }
    module.EndFunction();

    std::string asyncFunctionName = "TestAsync";
    auto asyncInvokerFunction = module.BeginFunction(asyncFunctionName, VariableType::Float, args);
    {
        llvm::Value* asyncArg = asyncInvokerFunction.GetFunctionArgument("x");
        auto task = asyncInvokerFunction.StartAsyncTask(taskFunction, { asyncArg });
        task.Wait(asyncInvokerFunction);
        asyncInvokerFunction.Return(task.GetReturnValue(asyncInvokerFunction));
    }
    module.EndFunction();

    // print the generated code
    // module.DebugDump();
    module.WriteToFile("asynctest.ll");
    
    IRExecutionEngine executionEngine(std::move(module));
    UnaryScalarFloatFunction syncCompiledFunction = (UnaryScalarFloatFunction)executionEngine.ResolveFunctionAddress(syncFunctionName);
    UnaryScalarFloatFunction asyncCompiledFunction = (UnaryScalarFloatFunction)executionEngine.ResolveFunctionAddress(asyncFunctionName);

    std::vector<float> data({ 1.1f, 2.1f, 3.1f, 4.1f, 5.1f });
    std::vector<float> computedResult;
    std::vector<float> syncCompiledResult;
    std::vector<float> asyncCompiledResult;
    for (auto x : data)
    {
        computedResult.push_back(x + 5.0);
        syncCompiledResult.push_back(syncCompiledFunction(x));
        asyncCompiledResult.push_back(asyncCompiledFunction(x));
        std::cout << (x+5.0) << ", " << syncCompiledFunction(x) << ", " << asyncCompiledFunction(x) << std::endl;
    }
    testing::ProcessTest("Testing compilable syncFunction", testing::IsEqual(computedResult, syncCompiledResult));
    testing::ProcessTest("Testing compilable asyncFunction", testing::IsEqual(computedResult, asyncCompiledResult));
}

//
// TestParallelTasks
//
void TestParallelTasks(bool parallel, bool useThreadPool)
{
    std::cout << "Testing parallel tasks in " << (parallel ? (useThreadPool ? "threadpool" : "async") : "deferred") << " mode" << std::endl;
    CompilerOptions options;
    options.optimize = false;
    options.targetDevice.deviceName = "host";
    options.parallelize = parallel;
    options.useThreadPool = useThreadPool;
    IRModuleEmitter module("ThreadPoolTest", options);
    module.DeclarePrintf();

    // Types
    auto& context = module.GetLLVMContext();
    llvm::Type* int32Type = llvm::Type::getInt32Ty(context);
    llvm::Type* int32PtrType = int32Type->getPointerTo();

    // Common stuff for both functions
    NamedLLVMTypeList args;
    args.push_back({ "arr", int32PtrType });
    args.push_back({ "begin", int32Type });
    args.push_back({ "end", int32Type });

    //
    // Task function
    //    
    std::string taskFunctionName = "TestTaskFunction";
    auto taskFunction = module.BeginFunction(taskFunctionName, int32Type, {int32PtrType, int32Type, int32Type});
    {
        auto arguments = taskFunction.Arguments().begin();
        auto arr = &(*arguments++);
        auto begin = &(*arguments++);
        auto end = &(*arguments++);
        if(options.targetDevice.IsWindows())
        {
            taskFunction.Printf("Task\tbegin: %d\tend: %d\n", {begin, end});
        }
        else
        {
            taskFunction.Printf("[%x Task]\tbegin: %d\tend: %d\n", {taskFunction.PthreadSelf(), begin, end});
        }
        auto loop = taskFunction.ForLoop();
        loop.Begin(begin, end, taskFunction.Literal<int>(1));
        {
            auto i = loop.LoadIterationVariable();
            taskFunction.SetValueAt(arr, i, i);
        }
        loop.End();
        taskFunction.Return(end);
    }
    module.EndFunction();

    // Function to submit tasks to thread pool
    int desiredResult = 0;
    std::string testThreadPoolFunctionName = "TestThreadPool";
    auto testThreadPoolFunction = module.BeginFunction(testThreadPoolFunctionName, int32Type);
    {
        const int arraySize = 100;
        const int numTasks = 5;
        const int taskSize = (arraySize-1) / numTasks + 1;
        auto data = testThreadPoolFunction.Variable(int32Type, arraySize);
        std::vector<std::vector<llvm::Value*>> taskArrayArgs;
        for(int index = 0; index < numTasks; ++index)
        {
            auto begin = index*taskSize;
            auto end = std::min(begin+taskSize, arraySize);
            desiredResult += end;
            taskArrayArgs.push_back({data, testThreadPoolFunction.Literal<int>(begin), testThreadPoolFunction.Literal<int>(end)});
        }
        auto tasks = testThreadPoolFunction.StartTasks(taskFunction, taskArrayArgs);
        testThreadPoolFunction.Print("[Main]    \tDone submitting tasks\n");
        
        tasks.WaitAll(testThreadPoolFunction);
        testThreadPoolFunction.Print("[Main]    \tDone waiting for tasks\n");
        
        auto forLoop = testThreadPoolFunction.ForLoop();
        forLoop.Begin(arraySize);
        {
            auto i = forLoop.LoadIterationVariable();
            testThreadPoolFunction.Printf("[Main]    \tdata[%d] = %d\n", {i, testThreadPoolFunction.ValueAt(data, i)});
        }
        forLoop.End();

        llvm::Value* sum = nullptr;
        for(size_t i = 0; i < numTasks; ++i)
        {
            auto task = tasks.GetTask(testThreadPoolFunction, i);
            auto returnValue = task.GetReturnValue(testThreadPoolFunction);
            testThreadPoolFunction.Printf("Task %d return value: %d\n", {testThreadPoolFunction.Literal<int>(i), returnValue});
            if(sum == nullptr)
            {
                sum = returnValue;
            }
            else
            {
                sum = testThreadPoolFunction.Operator(emitters::GetAddForValueType<int>(), sum, returnValue);
            }
        }

        testThreadPoolFunction.Return(sum);
    }
    module.EndFunction();
    
    try
    {
        IRExecutionEngine executionEngine(std::move(module));

        // Call the function
        auto threadPoolFunction = (IntFunction)executionEngine.ResolveFunctionAddress(testThreadPoolFunctionName);
        auto result = threadPoolFunction();
        std::cout << "Called parallel tasks, result: " << result << std::endl;
        testing::ProcessTest("Testing compilable async function", testing::IsEqual(result, desiredResult));
}
    catch(utilities::Exception& exception)
    {
        std::cout << "Error, got exception:\n" << exception.GetMessage() << std::endl;
        throw;
    }

}
