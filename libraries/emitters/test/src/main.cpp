////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (emitters_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IREmitterTest.h"
#include "IRFunctionTest.h"

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

    // From IRFunctionTest.h
    TestIRAddFunction();
    TestIRFunction();
}

int main(int argc, char* argv[])
{
    TestIR();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
