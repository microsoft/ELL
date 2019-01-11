////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestTransformData.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestTransformData.h"
#include "TransformData.h"

#include <testing/include/testing.h>

#include <utilities/include/MemoryLayout.h>

#include <iostream>
#include <numeric>
#include <vector>

using namespace ell;
using namespace ell::testing;

void TestTransformData()
{
    FailOnException(TestRemovePadding);
}

void TestRemovePaddingNoPadding()
{
    utilities::MemoryLayout layout({ 2, 2, 2 });
    std::vector<float> data(layout.GetMemorySize());
    std::iota(data.begin(), data.end(), 0);
    // Now data is a 2x2x2 thing containing:
    // 0,1    2,3
    // 4,5    6,7
    auto unpadded = RemovePadding(data, layout);
    testing::ProcessTest("RemovePadding of unpadded memory", testing::IsEqual(unpadded, data));
}

void TestRemovePaddingWithPadding()
{
    utilities::MemoryLayout layout(utilities::MemoryShape{ 2, 2, 2 }, utilities::MemoryShape{ 1, 1, 0 });
    std::vector<float> data(layout.GetMemorySize());
    std::iota(data.begin(), data.end(), 0);
    // Now data is a 4x4x2 thing containing:
    //  0,1      2,3     4,5     6,7
    //  8,9    10,11   12,13   14,15
    // 16,17   18,19   20,21   22,23
    // 24,25   26,27   28,29   30,31
    //
    // where the "active" part is just:
    //  10,11   12,13
    //  18,19   20,21

    auto unpadded = RemovePadding(data, layout);
    std::vector<float> expected = { 10, 11, 12, 13, 18, 19, 20, 21 };
    testing::ProcessTest("RemovePadding of padded memory", testing::IsEqual(unpadded, expected));
}

void TestRemovePadding()
{
    TestRemovePaddingNoPadding();
    TestRemovePaddingWithPadding();
}
