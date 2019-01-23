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
        { 8.7f, 7.8f, 0.6f, 8.8f },
        { 8.3f, 0.7f, 4.0f, 4.0f },
        { 6.6f, 1.1f, 7.1f, 0.1f },
        { 6.8f, 2.7f, 7.7f, 6.9f },
        { 5.4f, 6.6f, 2.9f, 1.1f },
        { 9.3f, 4.6f, 8.4f, 2.8f },
        { 9.5f, 7.7f, 0.1f, 2.1f },
        { 9.0f, 6.7f, 4.3f, 7.0f },
        { 1.1f, 6.3f, 2.3f, 3.7f },
        { 5.4f, 3.0f, 3.0f, 0.0f }
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
