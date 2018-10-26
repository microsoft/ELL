////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MemoryLayout_test.cpp (utilities)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MemoryLayout_test.h"

// utilities
#include "MemoryLayout.h"

// testing
#include "testing.h"

namespace ell
{
using namespace utilities;

void TestDimensionOrder()
{
    MemoryLayout layout({7, 5, 3}, ChannelMajorTensorOrder);

    bool ok = true;
    ok &= testing::IsEqual(layout.GetPhysicalDimension(2), 0);
    ok &= testing::IsEqual(layout.GetPhysicalDimension(0), 1);
    ok &= testing::IsEqual(layout.GetPhysicalDimension(1), 2);
    testing::ProcessTest("MemoryLayout::GetPhysicalDimension", ok);

    ok = true;
    ok &= testing::IsEqual(layout.GetLogicalDimension(0), 2);
    ok &= testing::IsEqual(layout.GetLogicalDimension(1), 0);
    ok &= testing::IsEqual(layout.GetLogicalDimension(2), 1);
    testing::ProcessTest("MemoryLayout::GetLogicalDimension", ok);
}

}