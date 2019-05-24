////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadTestData.h (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataUtils.h"

#include <math/include/Vector.h>

#include <string>

ell::MultiClassDataContainer LoadSmallMNISTDataContainer();
ell::MultiClassDataContainer LoadTrivialConvDataContainer();
ell::MultiClassDataContainer LoadTrivialConvDataContainer2();

ell::UnlabeledDataContainer GetSimpleStatsDataset();
ell::math::RowVector<double> GetSimpleStatsDatasetMean();
ell::math::RowVector<double> GetSimpleStatsDatasetStdDev();
double GetSimpleStatsDatasetScalarMean();
double GetSimpleStatsDatasetScalarStdDev();
