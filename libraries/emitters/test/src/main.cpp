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

using namespace ell;

void TestIR()
{
    // From IREmitterTest.h
    TestLLVM();
    TestLLVMShiftRegister();
    TestIfElseComplex();
    TestIfElseBlockRegions(false);
    TestIfElseBlockRegions(true);
    TestLogical();
    TestMutableConditionForLoop();
    TestMetadata();
    TestHeader();
    TestTwoEmitsInOneSession();
    TestStruct();

    // From IRFunctionTest.h
    TestIRAddFunction();
    TestIRFunction();
}

void TestAsyncEmitter()
{
    TestIRAsyncTask();
}

void TestPosixEmitter()
{
    TestPthreadSelf();
    TestPthreadCreate();
}

int main(int argc, char* argv[])
{
    TestStruct();
    return 0;

    TestIR();
    TestAsyncEmitter();
    TestPosixEmitter();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
