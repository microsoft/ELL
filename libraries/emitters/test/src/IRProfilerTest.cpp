////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRProfilerTest.cpp (emitters_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRProfilerTest.h"

#include <emitters/include/CompilerOptions.h>
#include <emitters/include/EmitterException.h>
#include <emitters/include/EmitterTypes.h>
#include <emitters/include/IREmitter.h>
#include <emitters/include/IRExecutionEngine.h>
#include <emitters/include/IRFunctionEmitter.h>
#include <emitters/include/IRModuleEmitter.h>
#include <emitters/include/IRProfiler.h>
#include <emitters/include/Variable.h>

#include <testing/include/testing.h>

#include <utilities/include/Unused.h>

#include <string>
#include <vector>

#if 0
// re-include this for debugging
#include <iostream>
#endif

using namespace ell;
using namespace ell::emitters;

//
// Tests
//

void TestProfileRegion()
{
    CompilerOptions options;
    options.optimize = false;
    options.profile = true;
    std::string moduleName = "CompilableIRFunction";
    IRModuleEmitter module(moduleName, options);
    module.DeclarePrintf();

    std::string functionName = "TestProfileRegion";
    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Double });
    auto function = module.BeginFunction(functionName, VariableType::Double, args);
    {
        // function.Print("Test function begin\n");
        auto x = function.LocalScalar(function.GetFunctionArgument("x"));

        IRProfileRegion region1(function, "TestRegion1");
        testing::ProcessTest("Testing IRProfileRegion time not set", !region1.IsStartTimeValid());
        region1.Enter();
        auto result1 = 5.0 * x;
        testing::ProcessTest("Testing IRProfileRegion time set", region1.IsStartTimeValid());
        region1.Exit();

        testing::ProcessTest("Testing IRProfileRegion time not set", !region1.IsStartTimeValid());
        region1.Enter();
        auto result2 = 5.0 * x + result1;

        // Do something time-consuming:
        int vecSize = 10000;
        int numIter = 100;
        auto vec = function.Variable(VariableType::Double, vecSize);
        function.For(numIter, [vec, vecSize](IRFunctionEmitter& function, auto) {
            auto dotSum = function.DotProduct(vecSize, vec, vec);
            UNUSED(dotSum);
        });
        testing::ProcessTest("Testing IRProfileRegion time set", region1.IsStartTimeValid());
        region1.Exit();

        IRProfileRegion region2(function, "TestRegion2");
        testing::ProcessTest("Testing IRProfileRegion time set", !region2.IsStartTimeValid());
        region2.Enter();
        testing::ProcessTest("Testing IRProfileRegion time set", region2.IsStartTimeValid());
        auto result3 = result2 + function.LocalScalar<double>(5.0);
        region2.Exit();
        testing::ProcessTest("Testing IRProfileRegion time not set", !region2.IsStartTimeValid());
        function.Return(result3);
    }
    module.EndFunction();

// When debugging, it can be helpful to dump the IR
#if 0
    module.DebugDump();
#endif
    auto getNumRegionsFunctionName = module.GetProfiler().GetGetNumRegionsFunctionName();
    auto getRegionInfoFunctionName = module.GetProfiler().GetGetRegionProfilingInfoFunctionName();
    auto resetRegionsFunctionName = module.GetProfiler().GetResetRegionProfilingInfoFunctionName();

    IRExecutionEngine executionEngine(std::move(module));

    using UnaryScalarDoubleFunctionType = double (*)(double);
    auto compiledFunction = (UnaryScalarDoubleFunctionType)executionEngine.ResolveFunctionAddress(functionName);

    // Check region count
    using GetCountFunctionType = int32_t (*)();
    auto getNumRegionsFunction = (GetCountFunctionType)executionEngine.ResolveFunctionAddress(getNumRegionsFunctionName);
    auto numRegions = getNumRegionsFunction();
    testing::ProcessTest("Testing profile regions", testing::IsEqual(numRegions, 2));

    // Execute the function a few times
    std::vector<double> data({ 1.1, 2.1, 3.1, 4.1, 5.1 });
    std::vector<double> compiledResult;
    for (auto x : data)
    {
        compiledResult.push_back(compiledFunction(x));
    }

    using VoidFunctionType = void (*)();
    using GetRegionFunctionType = ProfileRegionInfo* (*)(int32_t);
    auto getRegionInfoFunction = (GetRegionFunctionType)executionEngine.ResolveFunctionAddress(getRegionInfoFunctionName);

// Printing out the results can be helpful when debugging the test
#if 0
    // Manually extract the regions and print them
    for(int index = 0; index < numRegions; ++index)
    {
        auto region = getRegionInfoFunction(index);
        std::cout << "Region " << index << ", count: " << region->count << ", time: " << region->totalTime << std::endl;
    }

    // Call the built-in region-printing function
    auto printProfileResultsFunction = (VoidFunctionType)executionEngine.ResolveFunctionAddress(printRegionsFunctionName);
    printProfileResultsFunction();
#endif

    // Check the regions have been executed the number of times we expect
    auto r0 = getRegionInfoFunction(0);
    auto r1 = getRegionInfoFunction(1);

    testing::ProcessTest("Testing profile regions", testing::IsEqual(r0->count, 10));
    testing::ProcessTest("Testing profile regions", testing::IsEqual(r1->count, 5));
    testing::ProcessTest("Testing profile regions", r0->totalTime > r1->totalTime);

    // Now reset profiler info and verify count and time are zero
    auto resetProfileResultsFunction = (VoidFunctionType)executionEngine.ResolveFunctionAddress(resetRegionsFunctionName);
    resetProfileResultsFunction();
    r0 = getRegionInfoFunction(0);
    r1 = getRegionInfoFunction(1);

    testing::ProcessTest("Testing profile regions", testing::IsEqual(r0->count, 0));
    testing::ProcessTest("Testing profile regions", testing::IsEqual(r0->totalTime, 0.0));
    testing::ProcessTest("Testing profile regions", testing::IsEqual(r1->count, 0));
    testing::ProcessTest("Testing profile regions", testing::IsEqual(r1->totalTime, 0.0));
}
