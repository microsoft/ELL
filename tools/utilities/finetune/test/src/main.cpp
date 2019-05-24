////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestDataStatistics.h"
#include "TestDataUtils.h"
#include "TestModelOutputDataCache.h"
#include "TestModelUtils.h"
#include "TestOptimizationUtils.h"
#include "TestTransformData.h"

#include <testing/include/testing.h>

using namespace ell;

int main(int argc, char* argv[])
{
    TestDataStatistics();
    TestDataUtils();
    TestModelUtils();
    TestOptimizationUtils();
    TestTransformData();
    TestModelOutputDataCache();

    return testing::GetExitCode();
}
