////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadTestData.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadTestData.h"
#include "DataUtils.h"

#include <utilities/include/Exception.h>
#include <utilities/include/Files.h>

#include <iostream>

using namespace ell;

std::string GetMNISTDatasetPath()
{
    return "../../Datasets/ELL/MNIST/mnist.gsdf";
}

std::string GetSmallMNISTDatasetPath()
{
    return "../../Datasets/ELL/MNIST/mnist_1k.gsdf";
}

std::string GetTrivialConvDatasetPath()
{
    return "../../Datasets/ELL/misc/trivial_conv.gsdf";
}

std::string GetTrivialConvDataset2Path()
{
    return "../../Datasets/ELL/misc/trivial_conv2.gsdf";
}

std::string GetSmallBinaryDatasetPath()
{
    return "../../Datasets/ELL/MNIST/mnist_1k_binary.gsdf";
}

MultiClassDataContainer LoadSmallMNISTDataContainer()
{
    auto datasetPath = GetSmallMNISTDatasetPath();
    return LoadMultiClassDataContainer(datasetPath);
}

MultiClassDataContainer LoadTrivialConvDataContainer()
{
    auto datasetPath = GetTrivialConvDatasetPath();
    return LoadMultiClassDataContainer(datasetPath);
}

MultiClassDataContainer LoadTrivialConvDataContainer2()
{
    auto datasetPath = GetTrivialConvDataset2Path();
    return LoadMultiClassDataContainer(datasetPath);
}

UnlabeledDataContainer GetSimpleStatsDataset()
{
    std::vector<std::vector<float>> rawData = {
        { 8.7, 7.8, 0.6, 8.8 },
        { 8.3, 0.7, 4., 4. },
        { 6.6, 1.1, 7.1, 0.1 },
        { 6.8, 2.7, 7.7, 6.9 },
        { 5.4, 6.6, 2.9, 1.1 },
        { 9.3, 4.6, 8.4, 2.8 },
        { 9.5, 7.7, 0.1, 2.1 },
        { 9., 6.7, 4.3, 7. },
        { 1.1, 6.3, 2.3, 3.7 },
        { 5.4, 3., 3., 0. }
    };

    UnlabeledDataContainer result;
    for (auto x : rawData)
    {
        result.Add(x);
    }
    return result;
}

math::RowVector<double> GetSimpleStatsDatasetMean()
{
    return { 7.01, 4.72, 4.04, 3.65 };
}

math::RowVector<double> GetSimpleStatsDatasetStdDev()
{
    return { 2.45456717, 2.54236111, 2.73137328, 2.89801656 };
}

double GetSimpleStatsDatasetScalarMean()
{
    return 4.8549999999999995;
}

double GetSimpleStatsDatasetScalarStdDev()
{
    return 2.9633553617478956;
}
