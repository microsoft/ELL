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

// stl
#include <algorithm>
#include <numeric>

namespace ell
{
using namespace utilities;

void TestMemoryLayoutCtors()
{
    auto test = [](int rows, int columns, int rowPadding, int columnPadding) {
        bool ok = true;
        {
            MemoryLayout layout({ rows, columns });
            ok = testing::IsTrue(
                layout.NumDimensions() == 2 && static_cast<int>(layout.NumElements()) == rows * columns &&
                layout.GetActiveSize() == MemoryShape({ rows, columns }) &&
                layout.GetExtent() == MemoryShape({ rows, columns }) && layout.GetOffset() == MemoryShape({ 0, 0 }) &&
                layout.GetCumulativeIncrement() == MemoryShape({ columns, 1 }));
        }
        if (!ok)
        {
            return ok;
        }
        {
            MemoryLayout layout({ rows, columns }, MemoryShape{ rowPadding, columnPadding });
            ok = testing::IsTrue(
                layout.NumDimensions() == 2 && static_cast<int>(layout.NumElements()) == rows * columns &&
                layout.GetActiveSize() == MemoryShape({ rows, columns }) &&
                layout.GetExtent() == MemoryShape({ rows + (2 * rowPadding), columns + (2 * columnPadding) }) &&
                layout.GetOffset() == MemoryShape({ rowPadding, columnPadding }) &&
                layout.GetCumulativeIncrement() == MemoryShape({ columns + (2 * columnPadding), 1 }));
        }
        if (!ok)
        {
            return ok;
        }
        try
        {
            if (rowPadding > 0 || columnPadding > 0)
            {
                MemoryLayout layout({ rows, columns }, { 0, 0 }, MemoryShape{ rowPadding, columnPadding });
                ok = false;
            }
        }
        catch (InputException&)
        {}
        if (!ok)
        {
            return ok;
        }
        {
            MemoryLayout layout({ rows, columns },
                                { rows + (2 * rowPadding), columns + (2 * columnPadding) },
                                MemoryShape{ rowPadding, columnPadding });
            ok = testing::IsTrue(
                layout.NumDimensions() == 2 && static_cast<int>(layout.NumElements()) == rows * columns &&
                layout.GetActiveSize() == MemoryShape({ rows, columns }) &&
                layout.GetExtent() == MemoryShape({ rows + (2 * rowPadding), columns + (2 * columnPadding) }) &&
                layout.GetOffset() == MemoryShape({ rowPadding, columnPadding }) &&
                layout.GetCumulativeIncrement() == MemoryShape({ columns + (2 * columnPadding), 1 }));
        }
        return ok;
    };

    bool ok = true;
    for (int rows = 0; rows < 3 && ok; ++rows)
    {
        for (int columns = 0; columns < 3 && ok; ++columns)
        {
            for (int rowPadding = 0; rowPadding < 2 && ok; ++rowPadding)
            {
                for (int columnPadding = 0; columnPadding < 2 && ok; ++columnPadding)
                {
                    ok &= test(rows, columns, rowPadding, columnPadding);
                }
            }
        }
    }
    testing::ProcessTest("Basic MemoryLayout ctor test", ok);
}

void TestMemoryLayoutSlice()
{
    auto testSlices = [](MemoryLayout layout) -> bool {
        bool ok = true;

        for (int sliceDimension = 0; sliceDimension < layout.NumDimensions() && ok; ++sliceDimension)
        {
            auto sliced = layout.GetSliceLayout(sliceDimension);

            ok = sliced.NumDimensions() == layout.NumDimensions() - 1;
            if (!ok)
            {
                break;
            }

            if (sliceDimension == 0)
            {
                ok = static_cast<int>(sliced.GetMemorySize()) ==
                     static_cast<int>(layout.GetMemorySize()) / layout.GetExtent(0);
            }
            else
            {
                ok = sliced.GetMemorySize() == layout.GetMemorySize();
            }

            for (int dimension = 0; dimension < sliceDimension && ok; ++dimension)
            {
                ok = sliced.GetActiveSize(dimension) == layout.GetActiveSize(dimension);
                ok &= sliced.GetCumulativeIncrement(dimension) == layout.GetCumulativeIncrement(dimension);
                if (dimension == sliceDimension - 1)
                {
                    ok &= sliced.GetExtent(dimension) == layout.GetExtent(dimension) * layout.GetExtent(dimension + 1);
                }
                else
                {
                    ok &= sliced.GetExtent(dimension) == layout.GetExtent(dimension);
                }
            }
            for (int dimension = sliceDimension; dimension < sliced.NumDimensions() && ok; ++dimension)
            {
                ok = sliced.GetActiveSize(dimension) == layout.GetActiveSize(dimension + 1);
                ok &= sliced.GetCumulativeIncrement(dimension) == layout.GetCumulativeIncrement(dimension + 1);
                ok &= sliced.GetExtent(dimension) == layout.GetExtent(dimension + 1);
            }
        }

        return ok;
    };

    constexpr int rows = 3, columns = 5, channels = 7, outerExtent = 4;

    bool ok = true;
    std::vector<int> physicalSize;
    for (auto i : { rows, columns, channels, outerExtent })
    {
        physicalSize.push_back(i);
        std::vector<int> order(physicalSize.size());
        std::iota(order.begin(), order.end(), 0);

        do
        {
            MemoryLayout layout(physicalSize, DimensionOrder(order));
            ok = testSlices(layout);
        } while (std::next_permutation(order.begin(), order.end()) && ok);
        if (!ok)
        {
            break;
        }
    }
    testing::ProcessTest("MemoryLayout::Slice test", ok);
}

void TestDimensionOrder()
{
    MemoryLayout layout({ 7, 5, 3 }, ChannelMajorTensorOrder);

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

} // namespace ell