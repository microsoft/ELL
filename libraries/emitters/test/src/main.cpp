////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (emitters_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "IREmitterTest.h"

// testing
#include "testing.h"

using namespace ell;

void TestIR()
{
    TestLLVM();
    TestLLVMShiftRegister();
    TestIfElseComplex();
    TestIfElseBlockRegions(false);
    TestIfElseBlockRegions(true);
    TestLogical();
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
