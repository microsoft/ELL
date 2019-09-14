////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (emitters_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AsyncEmitterTest.h"
#include "IREmitterTest.h"
#include "IRFunctionTest.h"
#include "IRProfilerTest.h"
#include "PosixEmitterTest.h"
#include "StdlibEmitterTest.h"

#include <testing/include/testing.h>

// set to 1 if you want to test emitted IR that is async
#define TEST_THREAD_EMITTED_IR 0

using namespace ell;

void TestIR()
{
    // From IREmitterTest.h
    TestIREmitter();

    TestEmitLLVM();
    TestLLVMShiftRegister();
    TestHighLevelNestedIf();
    TestMixedLevelNestedIf();
    TestLogicalAnd();
    TestLogicalOr();
    TestLogicalNot();
    TestBinaryOperations();
    TestBinaryLogicalOperations();
    TestForLoop();
    TestWhileLoopWithVariableCondition();
    TestWhileLoopWithFunctionCondition();
    TestWhileLoopWithInt32Condition();
    TestMetadata();
    TestHeader();
    TestTwoEmitsInOneSession();
    TestStruct();
    TestDuplicateStructs();

    // if/then constructs
    TestScopedIf();
    TestScopedIfElse();
    TestScopedIfElse2();
    TestElseIfWithComputedCondition();

    TestCastValue();
    TestCastToConditionalBool();
}

void TestIRFunction()
{
    TestIRAddFunction();
    TestCompilableFunction();
    TestStringCompareFunction();
    TestAllocaPlacement();
}

void TestAsyncEmitter()
{
    TestIRAsyncTask(false); // don't use threads
    TestIRAsyncTask(true); // do use threads (if available)

    TestParallelTasks(false, false); // deferred mode (no threads)
    TestParallelTasks(true, false); // async mode (always spin up a new thread)
    // TestParallelTasks(true, true);   // threadpool mode -- threadpool sometimes crashes or hangs when run in the JIT

    //
    TestParallelFor(0, 100, 1, false);
    TestParallelFor(0, 100, 2, false);
    TestParallelFor(10, 90, 1, false);
    TestParallelFor(10, 90, 2, false);
    TestParallelFor(10, 90, 3, false);
    TestParallelFor(30, 40, 11, false);
    TestParallelFor(0, 100, 1, true);
    TestParallelFor(0, 100, 2, true);
    TestParallelFor(10, 90, 1, true);
    TestParallelFor(10, 90, 2, true);
    TestParallelFor(10, 90, 3, true);
    TestParallelFor(30, 40, 11, true);
}

void TestPosixEmitter()
{
    TestPthreadSelf();
    TestPthreadCreate();
}

void TestProfiler()
{
    TestProfileRegion();
}

void TestStdlibEmitter()
{
    TestIRMallocFunction();
}

int main()
{
    TestIR();
    TestIRFunction();
    TestAsyncEmitter();
    TestPosixEmitter();
    TestProfiler();
    TestStdlibEmitter();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
