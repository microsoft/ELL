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
    TestMutableConditionForLoop();
    TestWhileLoop();
    TestMetadata();
    TestHeader();
    TestTwoEmitsInOneSession();
    TestStruct();
    TestDuplicateStructs();

    // From IRFunctionTest.h
    TestIRAddFunction();
    TestIRFunction();
}

void TestAsyncEmitter()
{
    TestIRAsyncTask(false); // don't use threads
    TestIRAsyncTask(true);  // do use threads (if available)
}

void TestPosixEmitter()
{
    TestPthreadSelf();
    TestPthreadCreate();
}

int main()
{
    TestIR();

    // The tests below crash when run through the JIT
#if TEST_THREAD_EMITTED_IR
    TestPosixEmitter();
    TestAsyncEmitter();
#endif // TEST_THREAD_EMITTED_IR

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
