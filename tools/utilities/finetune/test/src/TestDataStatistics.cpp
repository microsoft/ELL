////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestDataStatistics.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestDataStatistics.h"
#include "LoadTestData.h"

#include "DataStatistics.h"

#include <math/include/Vector.h>

#include <testing/include/testing.h>

#include <utilities/include/Exception.h>

// stl
#include <iostream>

using namespace ell;
using namespace ell::testing;

// Tests
void TestDataStatistics()
{
    FailOnException(TestGetDataStatistics);
    FailOnException(TestGetScalarDataStatistics);
    FailOnException(TestGetNormalizedData);
}

void TestGetDataStatistics()
{
    auto dataset = GetSimpleStatsDataset();
    auto expectedMean = GetSimpleStatsDatasetMean();
    auto expectedStdDev = GetSimpleStatsDatasetStdDev();
    auto stats = GetDataStatistics(dataset);
    const double tolerance = 1e-7;
    ProcessTest("Checking mean", testing::IsEqual(expectedMean.ToArray(), stats.mean.ToArray(), tolerance));
    ProcessTest("Checking std deviation", testing::IsEqual(expectedStdDev.ToArray(), stats.stdDev.ToArray(), tolerance));
}

void TestGetScalarDataStatistics()
{
    auto dataset = GetSimpleStatsDataset();
    auto expectedMean = GetSimpleStatsDatasetScalarMean();
    auto expectedStdDev = GetSimpleStatsDatasetScalarStdDev();
    auto stats = GetScalarDataStatistics(dataset);
    const double tolerance = 1e-7;
    ProcessTest("Checking scalar mean", testing::IsEqual(expectedMean, stats.mean[0], tolerance));
    ProcessTest("Checking scalar std deviation", testing::IsEqual(expectedStdDev, stats.stdDev[0], tolerance));
}

void TestGetNormalizedData()
{
    auto dataset = GetSimpleStatsDataset();
    auto originalStats = GetDataStatistics(dataset);
    const double tolerance = 1e-6;
    math::RowVector<double> zeroMean(originalStats.mean.Size());
    math::RowVector<double> unitVariance(originalStats.variance.Size());
    zeroMean.Fill(0.0);
    unitVariance.Fill(1.0);

    auto normalizedDataset = GetNormalizedData(dataset, originalStats);
    auto normalizedStats = GetDataStatistics(normalizedDataset);
    ProcessTest("Checking normalized mean", testing::IsEqual(zeroMean.ToArray(), normalizedStats.mean.ToArray(), tolerance));
    ProcessTest("Checking normalized variance", testing::IsEqual(unitVariance.ToArray(), normalizedStats.variance.ToArray(), tolerance));

    auto reverseNormalizedDataset = GetReverseNormalizedData(normalizedDataset, originalStats);
    auto reverseNormalizedStats = GetDataStatistics(reverseNormalizedDataset);
    ProcessTest("Checking reverse normalized mean", testing::IsEqual(originalStats.mean.ToArray(), reverseNormalizedStats.mean.ToArray(), tolerance));
    ProcessTest("Checking reverse normalized variance", testing::IsEqual(originalStats.variance.ToArray(), reverseNormalizedStats.variance.ToArray(), tolerance));
}
