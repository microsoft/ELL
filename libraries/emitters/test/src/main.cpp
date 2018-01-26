////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (emitters_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AsyncEmitterTest.h"
#include "InterfaceWriterTest.h"
#include "IREmitterTest.h"
#include "IRFunctionTest.h"
#include "PosixEmitterTest.h"

// testing
#include "testing.h"

// set to 1 if you want to test emitted IR that is async
#define TEST_THREAD_EMITTED_IR 0

using namespace ell;

void TestIR()
{
    // From IREmitterTest.h
    TestIREmitter();

    TestLLVM();
    TestLLVMShiftRegister();
    TestIfElseComplex();
    TestIfElseBlockRegions(false);
    TestIfElseBlockRegions(true);
    TestLogical();
    TestForLoop();
    TestWhileLoop();
    TestMetadata();
    TestHeader();
    TestTwoEmitsInOneSession();
    TestStruct();
    TestDuplicateStructs();

    // New if constructs
    TestScopedIf();
    TestScopedIfElse();
    TestScopedIfElse2();

    // From IRFunctionTest.h
    TestIRAddFunction();
    TestIRFunction();
}

void TestAsyncEmitter()
{
    TestIRAsyncTask(false); // don't use threads
    TestIRAsyncTask(true);  // do use threads (if available)

    TestParallelTasks(false, false); // deferred mode (no threads)
    TestParallelTasks(true, false);  // async mode (always spin up a new thread)
    // TestParallelTasks(true, true);   // threadpool mode -- threadpool sometimes crashes or hangs when run in the JIT
}

void TestPosixEmitter()
{
    TestPthreadSelf();
    TestPthreadCreate();
}

void TestInterfaceWriter()
{
    TestCppHeader();
    TestSwigCallbackInterfaces();
    TestSwigCallbackHeader();
    TestSwigNoCallbackInterfaces();
    TestSwigNoCallbackHeader();
}

int main()
{
    TestIR();
    TestPosixEmitter();
    TestAsyncEmitter();
    TestInterfaceWriter();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
