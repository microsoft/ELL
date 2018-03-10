////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionTestData.h"

// dsp
#include "Convolution.h"

// math
#include "MathConstants.h"
#include "Matrix.h"
#include "MatrixOperations.h"
#include "Tensor.h"
#include "TensorOperations.h"
#include "Vector.h"
#include "VectorOperations.h"

// testing
#include "testing.h"

// utilities
#include "MillisecondTimer.h"

// stl
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace ell;

//
// Helper functions
//
template <typename ValueType>
std::ostream& operator<<(std::ostream& os, const std::vector<ValueType>& vec)
{
    for (auto x : vec)
        os << x << "  ";
    return os;
}

template <typename ValueType>
std::string GetSizeString(math::ConstRowMatrixReference<ValueType> input)
{
    return std::to_string(input.NumRows()) + " x " + std::to_string(input.NumColumns());
}

template <typename ValueType>
std::string GetSizeString(math::ConstChannelColumnRowTensorReference<ValueType> input)
{
    return std::to_string(input.NumRows()) + " x " + std::to_string(input.NumColumns()) + " x " + std::to_string(input.NumChannels());
}

template <typename ValueType>
std::string GetFilterSizeString(math::ConstRowMatrixReference<ValueType> filter)
{
    auto filterSize = filter.NumColumns();
    auto numFilters = filter.NumRows() / filterSize;
    return std::to_string(numFilters) + " " + std::to_string(filterSize) + " x " + std::to_string(filter.NumColumns());
}

template <typename ValueType>
std::string GetFilterSizeString(math::ConstChannelColumnRowTensorReference<ValueType> filters)
{
    auto filterSize = filters.NumColumns();
    auto numFilters = filters.NumRows() / filterSize;
    return std::to_string(numFilters) + " " + std::to_string(filterSize) + " x " + std::to_string(filterSize) + " x " + std::to_string(filters.NumChannels());
}

std::string GetConvAlgName(dsp::ConvolutionMethodOption alg)
{
    switch (alg)
    {
    case dsp::ConvolutionMethodOption::automatic:
        return "automatic";
    case dsp::ConvolutionMethodOption::simple:
        return "simple";
    case dsp::ConvolutionMethodOption::unrolled:
        return "unrolled";
    case dsp::ConvolutionMethodOption::diagonal:
        return "diagonal";
    case dsp::ConvolutionMethodOption::winograd:
        return "winograd";
    }
    return "";
}

// Helper function to avoid annoying double-to-float errors
template <typename ValueType, typename ValueType2>
ell::math::RowVector<ValueType> MakeVector(std::initializer_list<ValueType2> list)
{
    auto numElements = list.size();
    std::vector<ValueType> data;
    data.reserve(numElements);
    std::transform(list.begin(), list.end(), std::back_inserter(data), [](ValueType2 x) { return static_cast<ValueType>(x); });
    return ell::math::RowVector<ValueType>(data);
}

//
// Fill a vector with some "interesting" input signal data. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillInputVector(math::RowVector<ValueType>& input)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto size = static_cast<int>(input.Size());
    for (int index = 0; index < size; ++index)
    {
        double value = std::sin(2 * 2 * pi * index / size) + std::sin(2 * 3 * pi * index / size) + std::sin(2 * 9 * pi * index / size);
        value = std::trunc(value * 16) / 16.0;
        input[index] = static_cast<ValueType>(value);
    }
}

//
// Fill a vector with some "interesting" filter weights. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillFilterVector(math::RowVector<ValueType>& filter)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto size = static_cast<int>(filter.Size());
    for (int index = 0; index < size; ++index)
    {
        double value = 2 * std::sin(3.7 * pi * index / size) * std::cos(2 * 5 * pi * index / size) + std::cos(2 * 15 * pi * index / size);
        value = std::trunc(value * 16) / 16.0;
        filter[index] = static_cast<ValueType>(value);
    }
}

//
// Fill a matrix with some "interesting" input signal data. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillInputMatrix(math::RowMatrix<ValueType>& input)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto numRows = input.NumRows();
    const auto numColumns = input.NumColumns();
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (size_t columnIndex = 0; columnIndex < numColumns; ++columnIndex)
        {
            double value = std::sin(2 * 7 * pi * rowIndex / numRows) * std::cos(2 * 3 * pi * columnIndex / numColumns);
            value = std::trunc(value * 4) / 4.0;
            input(rowIndex, columnIndex) = static_cast<ValueType>(value);
        }
    }
}

//
// Fill a matrix with some "interesting" filter weights. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillFilterMatrix(math::RowMatrix<ValueType>& filter)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto numRows = filter.NumRows();
    const auto numColumns = filter.NumColumns();
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (size_t columnIndex = 0; columnIndex < numColumns; ++columnIndex)
        {
            double value = 2 * std::sin(2 * pi * rowIndex / numRows) * std::cos(2 * 2 * pi * columnIndex / numColumns);
            value = std::trunc(value * 4) / 4.0;
            value = (rowIndex == (numRows + 1) / 2) && (columnIndex == (numColumns + 1) / 2) ? 1.0 : 0.0;
            filter(rowIndex, columnIndex) = static_cast<ValueType>(value);
        }
    }
}

//
// Fill a tensor with some "interesting" input signal data. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillInputTensor(math::ChannelColumnRowTensor<ValueType>& input)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto numRows = input.NumRows();
    const auto numColumns = input.NumColumns();
    const auto numChannels = input.NumChannels();
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (size_t columnIndex = 0; columnIndex < numColumns; ++columnIndex)
        {
            for (size_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
            {
                double value = std::sin(2 * 7 * pi * rowIndex / numRows) * std::cos(2 * 3 * pi * columnIndex / numColumns) + 0.2 * std::sin(2 * 0.1 * pi * channelIndex / numChannels);
                value = std::trunc(value * 4) / 4.0;
                input(rowIndex, columnIndex, channelIndex) = static_cast<ValueType>(value);
            }
        }
    }
}

//
// Fill a tensor with some "interesting" filter weights. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillFiltersTensor(math::ChannelColumnRowTensor<ValueType>& filters, size_t numFilters)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto numRows = filters.NumRows() / numFilters;
    const auto numColumns = filters.NumColumns();
    const auto numChannels = filters.NumChannels();
    for (size_t filterIndex = 0; filterIndex < numFilters; ++filterIndex)
    {
        for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
        {
            for (size_t columnIndex = 0; columnIndex < numColumns; ++columnIndex)
            {
                for (size_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
                {
                    double value = 2 * std::sin(2 * pi * rowIndex / numRows) * std::cos(2 * 2 * pi * columnIndex / numColumns) + 0.3 * std::sin(2 * pi * channelIndex / numChannels) * std::cos(2 * 1.5 * pi * filterIndex / numFilters);
                    value = std::trunc(value * 4) / 4.0;
                    filters((filterIndex * numRows) + rowIndex, columnIndex, channelIndex) = static_cast<ValueType>(value);
                }
            }
        }
    }
}

//
// Tests
//

template <typename ValueType>
void TestConv1D(dsp::ConvolutionMethodOption algorithm)
{
    ValueType epsilon = static_cast<ValueType>(1e-6);

    math::RowVector<ValueType> signal = MakeVector<ValueType>({ 0.42929697, 0.90317845, 0.84490289, 0.66174327, 0.10820399, 0.3511343, 0.58248869, 0.62674724, 0.11014194, 0.00132073, 0.58431646, 0.39873614, 0.40304155, 0.79139607, 0.97710827, 0.21268128 });
    math::RowVector<ValueType> filter = MakeVector<ValueType>({ 0.25, 0.5, 0.25 });
    math::RowVector<ValueType> reference = MakeVector<ValueType>({ 0.77013919, 0.81368187, 0.56914835, 0.30732139, 0.34824032, 0.53571473, 0.48653128, 0.21208796, 0.17427497, 0.39217245, 0.44620757, 0.49905383, 0.74073549, 0.73957347 });

    // Perform the convolution
    auto result = Convolve1D(signal, filter, algorithm);

    testing::ProcessTest("Testing convolution result size matches", result.Size() == reference.Size());
    if (result.Size() != reference.Size())
    {
        std::cerr << "Error: result sizes not equal, reference: " << reference.Size() << ", result: " << result.Size() << std::endl;
    }

    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, epsilon));
    if (!ok)
    {
        std::cout << "Incorrect result for 1D " << GetConvAlgName(algorithm) << " convolution " << std::endl;
#if 0
        // Useful for pinpointing errors during debugging:
        std::cerr << "Reference:\n" << reference << std::endl;
        std::cerr << "Computed: \n" << result << std::endl;
#endif
    }
}

template <typename ValueType>
void TestConv2D(dsp::ConvolutionMethodOption algorithm)
{
    ValueType epsilon = static_cast<ValueType>(1e-5);

    math::RowMatrix<ValueType> signal = GetReferenceMatrixSignal<ValueType>();
    math::RowMatrix<ValueType> filter = GetReferenceMatrixFilter<ValueType>();
    math::RowMatrix<ValueType> reference = GetReferenceMatrixConvolution<ValueType>();

    // Perform the convolution
    auto result = Convolve2D(signal, filter, algorithm);

    testing::ProcessTest("Testing convolution result size matches", (result.NumRows() == reference.NumRows()) && (result.NumColumns() == reference.NumColumns()));
    if (result.NumRows() != reference.NumRows())
    {
        std::cerr << "Error: result sizes not equal, reference: " << reference.Size() << ", result: " << result.Size() << std::endl;
    }

    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, epsilon));
    if (!ok)
    {
        std::cout << "Incorrect result for 2D matrix " << GetConvAlgName(algorithm) << " convolution on input of size " << signal.NumRows() << " x " << signal.NumColumns() << std::endl;
        math::RowMatrix<ValueType> diff(result);
        diff -= reference;

#if 0
        // Useful for pinpointing errors during debugging:

        std::cerr << "Reference:\n" << reference << std::endl;
        std::cerr << "Computed:\n" << result << std::endl;
        std::cerr << "Difference:  " << diff << std::endl;
#endif
        auto diffArray = diff.ToArray();
        std::cerr << "Max difference:  " << *std::max_element(diffArray.begin(), diffArray.end()) << std::endl;
    }
}

template <typename ValueType>
void TestConv1DVsSimple(size_t length, size_t filterSize, dsp::ConvolutionMethodOption algorithm)
{
    using Vector = math::RowVector<ValueType>;
    const ValueType epsilon = static_cast<ValueType>(1e-5);

    Vector signal(length);
    Vector filter(filterSize);

    FillInputVector(signal);
    FillFilterVector(filter);

    // Perform the convolution
    auto reference = Convolve1D(signal, filter, dsp::ConvolutionMethodOption::simple);
    auto result = Convolve1D(signal, filter, algorithm);

    // Compare results
    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, epsilon));
    if (!ok)
    {
        std::cout << "Incorrect result for 1D " << GetConvAlgName(algorithm) << " convolution on input of size " << signal.Size() << std::endl;
        std::cout << "Reference:\n"
                  << reference << std::endl;
        std::cout << std::endl;
        std::cout << "Computed:\n"
                  << result << std::endl;
        auto referenceArray = reference.ToArray();
        auto resultArray = result.ToArray();
        auto size = referenceArray.size();
        std::vector<ValueType> diffArray(size);
        for (size_t index = 0; index < size; ++index)
        {
            diffArray[index] = referenceArray[index] - resultArray[index];
        }

#if 0
        // Useful for pinpointing errors during debugging:
        std::cout << "Difference:  " << diffArray() << std::endl;
#endif
        std::cout << "Max difference:  " << *std::max_element(diffArray.begin(), diffArray.end()) << std::endl;
    }
}

template <typename ValueType>
void TestConv2DMatrixVsSimple(size_t numRows, size_t numColumns, size_t filterSize, dsp::ConvolutionMethodOption algorithm)
{
    using Matrix = math::RowMatrix<ValueType>;

    const ValueType epsilon = static_cast<ValueType>(1e-5);

    const auto filterRows = filterSize;
    const auto filterColumns = filterSize;
    Matrix signal(numRows, numColumns);
    Matrix filter(filterRows, filterColumns);

    FillInputMatrix(signal);
    FillFilterMatrix(filter);

    // Perform the convolution
    auto reference = Convolve2D(signal, filter, dsp::ConvolutionMethodOption::simple);
    auto result = Convolve2D(signal, filter, algorithm);

    // Compare results
    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, epsilon));
    if (!ok)
    {
        std::cout << "Incorrect result for 2D matrix "
                  << " " << GetConvAlgName(algorithm) << " convolution on input of size " << signal.NumRows() << " x " << signal.NumColumns() << std::endl;
        std::cout << "Reference:\n"
                  << reference << std::endl;
        std::cout << std::endl;
        std::cout << "Computed:\n"
                  << result << std::endl;
        auto referenceArray = reference.ToArray();
        auto resultArray = result.ToArray();
        auto size = referenceArray.size();
        std::vector<ValueType> diffArray(size);
        for (size_t index = 0; index < size; ++index)
        {
            diffArray[index] = referenceArray[index] - resultArray[index];
        }

#if 0
        // Useful for pinpointing errors during debugging:
        std::cout << "Difference:  " << diffArray << std::endl;
#endif
        auto minmax = std::minmax_element(diffArray.begin(), diffArray.end());
        std::cout << "Min difference:  " << *minmax.first << std::endl;
        std::cout << "Max difference:  " << *minmax.second << std::endl;
    }
}

template <typename ValueType>
void TestConv2DTensorVsSimple(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, dsp::ConvolutionMethodOption algorithm)
{
    using Tensor = math::ChannelColumnRowTensor<ValueType>;

    const ValueType epsilon = static_cast<ValueType>(1e-5);

    const auto filterRows = filterSize;
    const auto filterColumns = filterSize;
    Tensor signal(numRows, numColumns, numChannels);
    Tensor filters(numFilters * filterRows, filterColumns, numChannels);

    FillInputTensor(signal);
    FillFiltersTensor(filters, numFilters);

    // Perform the convolution
    auto reference = Convolve2D(signal, filters, static_cast<int>(numFilters), dsp::ConvolutionMethodOption::simple);
    auto result = Convolve2D(signal, filters, static_cast<int>(numFilters), algorithm);

    // Compare results
    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, epsilon));
    if (!ok)
    {
        std::cout << "Incorrect result for 2D tensor "
                  << " " << GetConvAlgName(algorithm) << " convolution on input of size " << signal.NumRows() << " x " << signal.NumColumns() << " x " << signal.NumChannels() << std::endl;
        // std::cout << "Reference:\n" << reference << std::endl;
        // std::cout << std::endl;
        // std::cout << "Computed:\n" << result << std::endl;
        auto referenceArray = reference.ToArray();
        auto resultArray = result.ToArray();
        auto size = referenceArray.size();
        std::vector<ValueType> diffArray(size);
        for (size_t index = 0; index < size; ++index)
        {
            diffArray[index] = referenceArray[index] - resultArray[index];
        }

#if 0
        // Useful for pinpointing errors during debugging:
        std::cout << "Difference:  " << diffArray() << std::endl;
#endif
        std::cout << "Max difference:  " << *std::max_element(diffArray.begin(), diffArray.end()) << std::endl;
    }
}

//
// Timing
//
template <typename ValueType>
void TimeConv1D(size_t signalSize, size_t numIterations, dsp::ConvolutionMethodOption algorithm)
{
    math::RowVector<ValueType> signal(signalSize);
    math::RowVector<ValueType> filter = { 0.25, 0.5, 0.25 };

    // Perform the convolution
    utilities::MillisecondTimer timer;
    for (size_t iter = 0; iter < numIterations; ++iter)
    {
        auto result = Convolve1D(signal, filter, algorithm);
    }
    auto duration = timer.Elapsed();

    std::cout << "Time to perform 1D "
              << " " << GetConvAlgName(algorithm) << " convolution: " << duration << " ms" << std::endl;
}

template <typename ValueType>
void TimeConv2D(size_t numRows, size_t numColumns, size_t numIterations, dsp::ConvolutionMethodOption algorithm)
{
    math::RowMatrix<ValueType> signal(numRows, numColumns);
    math::RowMatrix<ValueType> filter{ { 0.25, 0.5, 0.25 },
                                       { 0.5, 0.75, 0.5 },
                                       { 0.25, 0.5, 0.25 } };

    // Perform the convolution
    utilities::MillisecondTimer timer;
    for (size_t iter = 0; iter < numIterations; ++iter)
    {
        auto result = Convolve2D(signal, filter, algorithm);
    }
    auto duration = timer.Elapsed();

    std::cout << "Time to perform 2D "
              << " " << GetConvAlgName(algorithm) << " convolution on " << GetSizeString(signal) << " input with " << GetFilterSizeString(filter) << " filters: " << duration << " ms" << std::endl;
}

template <typename ValueType>
void TimeConv2DTensor(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, dsp::ConvolutionMethodOption algorithm)
{
    const auto filterRows = filterSize;
    const auto filterColumns = filterSize;
    math::ChannelColumnRowTensor<ValueType> signal(numRows, numColumns, numChannels);
    math::ChannelColumnRowTensor<ValueType> filters{ numFilters * filterRows, filterColumns, numChannels };

    // Perform the convolution
    utilities::MillisecondTimer timer;
    for (size_t iter = 0; iter < numIterations; ++iter)
    {
        auto result = Convolve2D(signal, filters, static_cast<int>(numFilters), algorithm);
    }
    auto duration = timer.Elapsed();

    std::cout << "Time to perform 2D " << GetConvAlgName(algorithm) << " tensor convolution on " << GetSizeString(signal) << " input with " << GetFilterSizeString(filters) << " filters: " << duration << " ms" << std::endl;
}

//
// Explicit instantiations
//

// 1D
template void TestConv1DVsSimple<float>(size_t size, size_t filterSize, dsp::ConvolutionMethodOption algorithm);
template void TestConv1DVsSimple<double>(size_t size, size_t filterSize, dsp::ConvolutionMethodOption algorithm);
template void TestConv1D<float>(dsp::ConvolutionMethodOption);
template void TestConv1D<double>(dsp::ConvolutionMethodOption);

// 2D (matrix)
template void TestConv2DMatrixVsSimple<float>(size_t numRows, size_t numColumns, size_t filterSize, dsp::ConvolutionMethodOption algorithm);
template void TestConv2DMatrixVsSimple<double>(size_t numRows, size_t numColumns, size_t filterSize, dsp::ConvolutionMethodOption algorithm);
template void TestConv2D<float>(dsp::ConvolutionMethodOption);
template void TestConv2D<double>(dsp::ConvolutionMethodOption);

// 2D (Tensor)
template void TestConv2DTensorVsSimple<float>(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, dsp::ConvolutionMethodOption algorithm);
template void TestConv2DTensorVsSimple<double>(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, dsp::ConvolutionMethodOption algorithm);

template void TimeConv1D<float>(size_t signalSize, size_t numIterations, dsp::ConvolutionMethodOption);
template void TimeConv1D<double>(size_t signalSize, size_t numIterations, dsp::ConvolutionMethodOption);
template void TimeConv2D<float>(size_t numRows, size_t numColumns, size_t numIterations, dsp::ConvolutionMethodOption);
template void TimeConv2D<double>(size_t numRows, size_t numColumns, size_t numIterations, dsp::ConvolutionMethodOption);

template void TimeConv2DTensor<float>(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, dsp::ConvolutionMethodOption algorithm);
template void TimeConv2DTensor<double>(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, dsp::ConvolutionMethodOption algorithm);
