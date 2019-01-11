////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestDataUtils.h"
#include "TestModelUtils.h"
#include "TestOptimizationUtils.h"
#include "TestTransformData.h"

#include <testing/include/testing.h>

using namespace ell;

int main(int argc, char* argv[])
{
    TestDataUtils();
    TestModelUtils();
    TestOptimizationUtils();
    TestTransformData();

    return testing::GetExitCode();
}
