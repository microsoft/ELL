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

using VoidFunction = void (*)();
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

    // Common stuff for both functions
    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Float });

    //
    // Regular function
    //
    std::string syncFunctionName = "TestSync";
    auto syncFunction = module.BeginFunction(syncFunctionName, VariableType::Float, args);
    {
        LLVMValue syncArg = syncFunction.GetFunctionArgument("x");
        LLVMValue syncSum = syncFunction.Operator(emitters::GetAddForValueType<float>(), syncArg, syncFunction.Literal<float>(5.0));
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
        LLVMValue asyncSum = taskFunction.Operator(emitters::GetAddForValueType<float>(), argVal, taskFunction.Literal<float>(5.0));
        taskFunction.Return(asyncSum);
    }
    module.EndFunction();

    std::string asyncFunctionName = "TestAsync";
    auto asyncInvokerFunction = module.BeginFunction(asyncFunctionName, VariableType::Float, args);
    {
        LLVMValue asyncArg = asyncInvokerFunction.GetFunctionArgument("x");
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

    // Types
    auto& context = module.GetLLVMContext();
    LLVMType int32Type = llvm::Type::getInt32Ty(context);
    LLVMType int32PtrType = int32Type->getPointerTo();

    // Common stuff for both functions
    NamedLLVMTypeList args;
    args.push_back({ "arr", int32PtrType });
    args.push_back({ "begin", int32Type });
    args.push_back({ "end", int32Type });

    //
    // Task function
    //
    std::string taskFunctionName = "TestTaskFunction";
    auto taskFunction = module.BeginFunction(taskFunctionName, int32Type, { int32PtrType, int32Type, int32Type });
    {
        auto arguments = taskFunction.Arguments().begin();
        auto arr = taskFunction.LocalArray(&(*arguments++));
        auto begin = &(*arguments++);
        auto end = &(*arguments++);

        taskFunction.For(begin, end, [arr](emitters::IRFunctionEmitter& taskFunction, auto i) {
            arr[i] = i;
        });

        taskFunction.Return(end);
    }
    module.EndFunction();

    // Function to submit tasks to thread pool
    int desiredResult = 0;
    std::string testThreadPoolFunctionName = "TestThreadPool";
    auto testThreadPoolFunction = module.BeginFunction(testThreadPoolFunctionName, VariableType::Int32);
    {
        const int arraySize = 100;
        const int numTasks = 5;
        const int taskSize = (arraySize - 1) / numTasks + 1;
        auto data = testThreadPoolFunction.Variable(VariableType::Int32, arraySize);
        std::vector<std::vector<LLVMValue>> taskArrayArgs;
        for (int index = 0; index < numTasks; ++index)
        {
            auto begin = index * taskSize;
            auto end = std::min(begin + taskSize, arraySize);
            desiredResult += end;
            taskArrayArgs.push_back({ data, testThreadPoolFunction.Literal<int>(begin), testThreadPoolFunction.Literal<int>(end) });
        }
        auto tasks = testThreadPoolFunction.StartTasks(taskFunction, taskArrayArgs);
        tasks.WaitAll(testThreadPoolFunction);

        auto sum = testThreadPoolFunction.LocalScalar();
        for (size_t i = 0; i < numTasks; ++i)
        {
            auto task = tasks.GetTask(testThreadPoolFunction, i);
            auto returnValue = task.GetReturnValue(testThreadPoolFunction);
            if (sum.IsValid())
            {
                sum = sum + returnValue;
            }
            else
            {
                sum = testThreadPoolFunction.LocalScalar(returnValue);
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
        testing::ProcessTest("Testing compilable async function", testing::IsEqual(result, desiredResult));
    }
    catch (utilities::Exception& exception)
    {
        std::cout << "Error, got exception:\n"
                  << exception.GetMessage() << std::endl;
        throw;
    }
}

//
// TestParallelFor
//
void TestParallelFor(int begin, int end, int increment, bool parallel)
{
    CompilerOptions options;
    options.optimize = false;
    options.targetDevice.deviceName = "host";
    options.parallelize = parallel;
    options.useThreadPool = true;
    IRModuleEmitter module("ParallelForTest", options);

    // Function to run test
    std::string functionName = "TestParallelFor";
    auto testParallelForFunction = module.BeginFunction(functionName, VariableType::Int32);
    {
        const int arraySize = end;
        auto data = testParallelForFunction.GetModule().GlobalArray(VariableType::Int32, "data", arraySize);

        // initialize the array to -1
        testParallelForFunction.For(arraySize, [data](IRFunctionEmitter& function, LLVMValue i) {
            function.SetValueAt(data, i, function.Literal<int>(-1));
        });

        testParallelForFunction.ParallelFor(begin, end, increment, {}, { data }, [](IRFunctionEmitter& function, LLVMValue i, std::vector<LLVMValue> capturedValues) {
            auto data = capturedValues[0];
            function.SetValueAt(data, i, i);
        });

        auto result = testParallelForFunction.Variable(VariableType::Int32, "result");
        testParallelForFunction.Store(result, testParallelForFunction.Literal<int>(0));
        testParallelForFunction.For(arraySize, [begin, end, increment, data, result](IRFunctionEmitter& function, LLVMValue i) {
            auto index = function.LocalScalar(i);
            auto val = function.LocalScalar(function.ValueAt(data, index));
            function.If((index >= begin) && (index < end) && (((val - begin) % increment) == 0) && (val != index), [result](IRFunctionEmitter& function) {
                function.Store(result, function.Literal(1));
            });
        });
        testParallelForFunction.Return(testParallelForFunction.Load(result));
    }
    module.EndFunction();

    try
    {
        IRExecutionEngine executionEngine(std::move(module));

        // Call the function
        auto functionPtr = (IntFunction)executionEngine.ResolveFunctionAddress(functionName);
        auto result = functionPtr();
        testing::ProcessTest("Testing compilable parallel for loop", testing::IsEqual(result, 0));
    }
    catch (utilities::Exception& exception)
    {
        std::cout << "Error, got exception:\n"
                  << exception.GetMessage() << std::endl;
        throw;
    }
}
