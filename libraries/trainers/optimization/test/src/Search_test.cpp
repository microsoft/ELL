////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Search_test.cpp (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <trainers/optimization/include/BinarySearch.h>
#include <trainers/optimization/include/ExponentialSearch.h>
#include <trainers/optimization/include/GoldenSectionSearch.h>

#include <testing/include/testing.h>

using namespace ell;
using namespace ell::trainers::optimization;

void TestExponentialSearch()
{
    auto f = [](double x) { return (x + 2) * (x - 2) * (x - 3); };

    Interval target1(3000, 4000);
    auto search1 = ExponentialSearch(f, { target1 });
    search1.Update(30);
    auto solutionValue1 = search1.GetBoundingValues();

    Interval target2(-5000, -4500);
    auto search2 = ExponentialSearch(f, { target2 });
    search2.Update(30);
    auto solutionValue2 = search2.GetBoundingValues();

    testing::ProcessTest("TestExponentialSearch", target1.Intersects(solutionValue1) && target2.Intersects(solutionValue2));
}

void TestBinarySearch()
{
    // find a unique solution
    Interval valueInterval1(16.0, 17.0);
    auto search1 = BinarySearch([](double x) { return x * x; }, { valueInterval1, { 0.0, 5.0 } });
    search1.Update(20);
    auto resultInterval1 = search1.GetCurrentSearchIntervalValues();
    testing::ProcessTest("TestBinarySearch", valueInterval1.Intersects(resultInterval1));

    search1.Reset({ valueInterval1, { -5.0, 0.0 } });
    search1.Update(20);
    auto resultInterval2 = search1.GetCurrentSearchIntervalValues();
    testing::ProcessTest("TestBinarySearch", resultInterval2.Intersects(valueInterval1));

    // find one of three possible solutions
    Interval valueInterval2(6, 6);
    auto search2 = BinarySearch([](double x) { return (x + 2) * (x - 2) * (x - 3); }, { valueInterval2, { -4.0, 4.0 } });
    search2.Update(10);
    auto searchInterval = search2.GetCurrentSearchInterval();
    const double s7 = std::sqrt(7);
    testing::ProcessTest("TestBinarySearch",
                         (searchInterval.Contains(1)) ||
                             (searchInterval.Contains(1 - s7)) ||
                             (searchInterval.Contains(1 + s7)));
}

void TestGoldenSectionSearch()
{
    auto search1 = GoldenSectionSearch([](double x) { return (x - 2) * x; }, { { -2.0, 2.0 } });
    search1.Update(20);
    testing::ProcessTest("TestGoldenSectionSearch", std::abs(search1.GetBestArgument() - 1) < 1.0e-4);

    auto search2 = GoldenSectionSearch([](double x) { return -(x - 2) * x; }, { { -2.0, 2.0 }, 0, Objective::maximize });
    search2.Update(20);
    testing::ProcessTest("TestGoldenSectionSearch", std::abs(search2.GetBestArgument() - 1) < 1.0e-4);
}