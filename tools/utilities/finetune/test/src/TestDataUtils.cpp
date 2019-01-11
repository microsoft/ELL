////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestDataUtils.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestDataUtils.h"
#include "DataUtils.h"

#include <testing/include/testing.h>

#include <iostream>

using namespace ell;
using namespace ell::testing;

namespace
{
UnlabeledDataContainer GetFeaturesData()
{
    UnlabeledDataContainer data;
    data.Add({ 1.0f, 2.0f, 3.0f, 4.0f });
    data.Add({ 2.0f, 3.0f, 4.0f, 5.0f });
    data.Add({ 2.0f, 4.0f, 6.0f, 8.0f });
    return data;
}

UnlabeledDataContainer GetLabelsData()
{
    UnlabeledDataContainer data;
    data.Add({ 2.0f, 10.0f });
    data.Add({ 2.0f, 14.0f });
    data.Add({ 0.0f, 20.0f });
    return data;
}

BinaryLabelDataContainer GetBinaryLabelDataContainer()
{
    BinaryLabelDataContainer data;
    data.push_back({ { 1.0f, 2.0f, 3.0f, 4.0f }, 1 });
    data.push_back({ { 2.0f, 3.0f, 4.0f, 5.0f }, -1 });
    data.push_back({ { 2.0f, 4.0f, 6.0f, 8.0f }, 1 });
    return data;
}

UnlabeledDataContainer GetBinaryPredictions()
{
    UnlabeledDataContainer data;
    data.Add({ 0.2 }); // correct
    data.Add({ -0.5 }); // correct
    data.Add({ -0.1 }); // incorrect
    return data;
}

MultiClassDataContainer GetMultiClassDataContainer()
{
    MultiClassDataContainer data;
    data.Add({ { 1.0f, 2.0f, 3.0f, 4.0f }, 0 });
    data.Add({ { 2.0f, 3.0f, 4.0f, 5.0f }, 1 });
    data.Add({ { 2.0f, 4.0f, 6.0f, 8.0f }, 2 });
    return data;
}

UnlabeledDataContainer GetMultiClassPredictions()
{
    UnlabeledDataContainer data;
    data.Add({ 1.1, 0.3, -0.5 }); // correct
    data.Add({ 0.4, 0.3, -0.5 }); // incorrect
    data.Add({ -0.2, 0.1, -0.5 }); // incorrect
    return data;
}

VectorLabelDataContainer GetVectorLabelDataContainer()
{
    VectorLabelDataContainer data;
    data.push_back({ { 1.0f, 2.0f, 3.0f, 4.0f }, { 0.0, 1.0 } });
    data.push_back({ { 2.0f, 3.0f, 4.0f, 5.0f }, { 1.0, 2.0 } });
    data.push_back({ { 2.0f, 4.0f, 6.0f, 8.0f }, { 2.0, 4.0 } });
    return data;
}

UnlabeledDataContainer GetImageData(int numRows, int numColumns, int numChannels)
{
    // Returns a dataset with a single image of data in row-major format
    // The value for each pixel is determined by its location: 100 * row + 10 * column + channel.
    //
    // So, a 3 x 4 x 2 image would have the values
    //
    //  000, 001    010, 011    020, 021    030, 031
    //
    //  100, 101    110, 111    120, 121    130, 131
    //
    //  200, 201    210, 211    220, 221    230, 231
    //
    // (where values for the different channels are written next to each other, separated by a comma)

    const int rowSize = numRows * numColumns * numChannels;
    UnlabeledDataContainer result;
    UnlabeledExample row(rowSize);
    int count = 0;
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numColumns; ++j)
        {
            for (int k = 0; k < numChannels; ++k)
            {
                row[count++] = static_cast<float>(100 * i + 10 * j + k);
            }
        }
    }
    result.Add(row);
    return result;
}
} // namespace

void TestDataUtils()
{
    // Tests not implemented yet: awaiting a place to store test datasets

    NoFailOnUnimplemented(TestLoadBinaryLabelDataContainer);
    NoFailOnUnimplemented(TestLoadMultiClassDataContainer);
    NoFailOnUnimplemented(TestLoadCifarDataContainer);
    NoFailOnUnimplemented(TestLoadMnistDataContainer);

    FailOnException(TestCreateVectorLabelDataContainer);
    FailOnException(TestBinaryGetDatasetInputs);
    FailOnException(TestMultiClassGetDatasetInputs);
    FailOnException(TestVectorLabelGetDatasetInputs);
    FailOnException(TestGetDatasetOutputs);
    FailOnException(TestGetImageMatrixDataset);
    FailOnException(TestGetUnrolledImageDataset);
    FailOnException(TestBinaryGetModelAccuracy);
    FailOnException(TestMultiClassGetModelAccuracy);
}

void TestLoadBinaryLabelDataContainer()
{
    throw TestNotImplementedException("TestLoadBinaryLabelDataContainer");
}

void TestLoadMultiClassDataContainer()
{
    throw TestNotImplementedException("TestLoadMultiClassDataContainer");
}

void TestLoadCifarDataContainer()
{
    throw TestNotImplementedException("TestLoadCifarDataContainer");
}

void TestLoadMnistDataContainer()
{
    throw TestNotImplementedException("TestLoadMnistDataContainer");
}

// Combining datasets
void TestCreateVectorLabelDataContainer()
{
    auto features = GetFeaturesData();
    auto labels = GetLabelsData();
    auto dataset = CreateVectorLabelDataContainer(features, labels);

    testing::ProcessTest("Testing CreateVectorLabelDataContainer", features.Size() == dataset.Size());
    testing::ProcessTest("Testing CreateVectorLabelDataContainer", features[1][2] == dataset[1].input[2]);
    testing::ProcessTest("Testing CreateVectorLabelDataContainer", labels[1][0] == dataset[1].output[0]);
}

// Splitting datasets
void TestBinaryGetDatasetInputs()
{
    auto data = GetBinaryLabelDataContainer();
    auto inputs = GetDatasetInputs(data);

    testing::ProcessTest("TestBinaryGetDatasetInputs", testing::IsEqual(data[0].input, inputs[0]));
    testing::ProcessTest("TestBinaryGetDatasetInputs", testing::IsEqual(data[1].input, inputs[1]));
    testing::ProcessTest("TestBinaryGetDatasetInputs", testing::IsEqual(data[2].input, inputs[2]));
}

void TestMultiClassGetDatasetInputs()
{
    auto data = GetMultiClassDataContainer();
    auto inputs = GetDatasetInputs(data);

    testing::ProcessTest("TestMultiClassGetDatasetInputs", testing::IsEqual(data[0].input, inputs[0]));
    testing::ProcessTest("TestMultiClassGetDatasetInputs", testing::IsEqual(data[1].input, inputs[1]));
    testing::ProcessTest("TestMultiClassGetDatasetInputs", testing::IsEqual(data[2].input, inputs[2]));
}

void TestVectorLabelGetDatasetInputs()
{
    auto data = GetVectorLabelDataContainer();
    auto inputs = GetDatasetInputs(data);

    testing::ProcessTest("TestVectorLabelGetDatasetInputs", testing::IsEqual(data[0].input, inputs[0]));
    testing::ProcessTest("TestVectorLabelGetDatasetInputs", testing::IsEqual(data[1].input, inputs[1]));
    testing::ProcessTest("TestVectorLabelGetDatasetInputs", testing::IsEqual(data[2].input, inputs[2]));
}

void TestGetDatasetOutputs()
{
    auto data = GetVectorLabelDataContainer();
    auto outputs = GetDatasetOutputs(data);

    testing::ProcessTest("TestVectorLabelGetDatasetOutputs", testing::IsEqual(data[0].output, outputs[0]));
    testing::ProcessTest("TestVectorLabelGetDatasetOutputs", testing::IsEqual(data[1].output, outputs[1]));
    testing::ProcessTest("TestVectorLabelGetDatasetOutputs", testing::IsEqual(data[2].output, outputs[2]));
}

// Convolution-related
void TestGetImageMatrixDataset()
{
    const int numRows = 3;
    const int numColumns = 4;
    const int numChannels = 2;
    auto imageData = GetImageData(numRows, numColumns, numChannels);
    auto imageMatrixData = GetImageMatrixDataset(imageData, numRows, numColumns, numChannels);

    testing::ProcessTest("TestGetImageMatrixDataset size", imageMatrixData.Size() == imageData.Size() * numRows * numColumns);
    testing::ProcessTest("TestGetImageMatrixDataset[0] size", imageMatrixData[0].Size() == numChannels);
    testing::ProcessTest("TestGetImageMatrixDataset[0]", testing::IsEqual(imageMatrixData[0].ToArray(), { 0.0, 1.0 }));
    testing::ProcessTest("TestGetImageMatrixDataset[0]", testing::IsEqual(imageMatrixData[8].ToArray(), { 200.0, 201.0 })); // row 2, col 0
}

void TestGetUnrolledImageDataset()
{
    const int numRows = 3;
    const int numColumns = 4;
    const int numChannels = 2;
    const int filterSize = 3;
    const int stride = 1;
    auto imageData = GetImageData(numRows, numColumns, numChannels);
    auto unrolledImageData = GetUnrolledImageDataset(imageData, numRows, numColumns, numChannels, filterSize, stride);

    // unrolledImageFeatureMatrix is a (output rows * output columns) x (filterSize * filterSize * numChannels)  matrix
    testing::ProcessTest("TestGetUnrolledImageDataset size", unrolledImageData.Size() == (numRows - filterSize + 1) * (numColumns - filterSize + 1));
    testing::ProcessTest("TestGetUnrolledImageDataset[0] size", unrolledImageData[0].Size() == imageData.Size() * filterSize * filterSize * numChannels);

    // Image:
    //
    //       channel 0               channel 1
    //   000  010  020  030      001  011  021  031
    //   100  110  120  130      101  111  121  131
    //   200  210  220  230      201  211  221  231
    //
    // Unrolled matrix: 1*2 x 3*3*2 == 18 x 2
    //
    // First filter (row):
    //   000  010  020           001  011  021
    //   100  110  120           101  111  121
    //   200  210  220           201  211  221
    //
    // Second filter (row):
    //        010  020  030           011  021  031
    //        110  120  130           111  121  131
    //        210  220  230           211  221  231
    //

    std::vector<float> row0 = { 0, 1, 10, 11, 20, 21, 100, 101, 110, 111, 120, 121, 200, 201, 210, 211, 220, 221 };
    std::vector<float> row1 = { 10, 11, 20, 21, 30, 31, 110, 111, 120, 121, 130, 131, 210, 211, 220, 221, 230, 231 };
    testing::ProcessTest("TestGetUnrolledImageDataset[0]", testing::IsEqual(unrolledImageData[0].ToArray(), row0));
    testing::ProcessTest("TestGetUnrolledImageDataset[1]", testing::IsEqual(unrolledImageData[1].ToArray(), row1));
}

// Evaluating
void TestBinaryGetModelAccuracy()
{
    auto data = GetBinaryLabelDataContainer();
    auto pred = GetBinaryPredictions();
    testing::ProcessTest("TestBinaryGetModelAccuracy", testing::IsEqual(GetModelAccuracy(data, pred), 2.0 / 3.0, 1e-6));
}

void TestMultiClassGetModelAccuracy()
{
    auto data = GetMultiClassDataContainer();
    auto pred = GetMultiClassPredictions();
    testing::ProcessTest("TestMultiClassGetModelAccuracy", testing::IsEqual(GetModelAccuracy(data, pred), 1.0 / 3.0, 1e-6));
}
