////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTest.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionTest.h"
#include "ConvolutionTestData.h"
#include "DSPTestUtilities.h"

// dsp
#include "Convolution.h"

// math
#include "MathConstants.h"
#include "Tensor.h"
#include "TensorOperations.h"
#include "Vector.h"
#include "VectorOperations.h"

// testing
#include "testing.h"

// utilities
#include "MillisecondTimer.h"

// stl
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace ell;

//
// Helper functions
//
namespace
{
template <typename ValueType>
math::ChannelColumnRowTensor<ValueType>& operator-=(math::ChannelColumnRowTensor<ValueType>& a, const math::ChannelColumnRowTensor<ValueType>& b)
{
    assert(a.NumRows() == b.NumRows());
    assert(a.NumColumns() == b.NumColumns());
    assert(a.NumChannels() == b.NumChannels());

    for (size_t i = 0; i < a.NumRows(); ++i)
    {
        for (size_t j = 0; j < a.NumColumns(); ++j)
        {
            for (size_t k = 0; k < a.NumChannels(); ++k)
            {
                a(i, j, k) -= b(i, j, k);
            }
        }
    }
    return a;
}

const double epsilon = 1e-6;
} // namespace

//
// Tests
//

template <typename ValueType>
void TestConv1D(dsp::ConvolutionMethodOption algorithm)
{
    math::RowVector<ValueType> signal = Get1DReferenceSignal<ValueType>();
    math::RowVector<ValueType> filter = Get1DReferenceFilter<ValueType>();
    math::RowVector<ValueType> reference = Get1DReferenceConvolutionResult<ValueType>();

    // Perform the convolution
    auto result = Convolve1D(signal, filter, algorithm);

    testing::ProcessTest("Testing convolution result size matches", result.Size() == reference.Size());
    if (result.Size() != reference.Size())
    {
        std::cerr << "Error: result sizes not equal, reference: " << reference.Size() << ", result: " << result.Size() << std::endl;
    }

    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, static_cast<ValueType>(epsilon)));
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
    auto signal = GetReferenceSignal<ValueType>();
    auto filter = GetReferenceFilter<ValueType>();
    auto reference = GetReferenceConvolutionResult<ValueType>();

    // Perform the convolution
    auto result = Convolve2D(signal, filter, 1, algorithm);

    testing::ProcessTest("Testing convolution result size matches", (result.NumRows() == reference.NumRows()) && (result.NumColumns() == reference.NumColumns()));
    if (result.NumRows() != reference.NumRows())
    {
        std::cerr << "Error: result sizes not equal, reference: " << reference.Size() << ", result: " << result.Size() << std::endl;
    }

    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, static_cast<ValueType>(epsilon)));
    if (!ok)
    {
        std::cout << "Incorrect result for 2D convolution " << GetConvAlgName(algorithm) << " convolution on input of size " << signal.NumRows() << " x " << signal.NumColumns() << std::endl;
        math::ChannelColumnRowTensor<ValueType> diff(result);
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
void TestConv2DSeparable(dsp::ConvolutionMethodOption algorithm)
{
    auto signal = GetSeparableReferenceSignal<ValueType>();
    auto filters = GetSeparableReferenceFilters<ValueType>();
    auto reference = GetSeparableReferenceConvolutionResult<ValueType>();

    // Perform the convolution
    auto result = Convolve2DDepthwiseSeparable(signal, filters, static_cast<int>(signal.NumChannels()), algorithm);

    testing::ProcessTest("Testing convolution result size matches", (result.NumRows() == reference.NumRows()) && (result.NumColumns() == reference.NumColumns()));
    if (result.NumRows() != reference.NumRows())
    {
        std::cerr << "Error: result sizes not equal, reference: " << reference.Size() << ", result: " << result.Size() << std::endl;
    }

    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, static_cast<ValueType>(epsilon)));
    if (!ok)
    {
        std::cout << "Incorrect result for separable 2D convolution " << GetConvAlgName(algorithm) << " convolution on input of size " << signal.NumRows() << " x " << signal.NumColumns() << std::endl;
        math::ChannelColumnRowTensor<ValueType> diff(result);
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
void TestConv1DVsSimple(int length, int filterSize, dsp::ConvolutionMethodOption algorithm)
{
    using Vector = math::RowVector<ValueType>;

    Vector signal(length);
    Vector filter(filterSize);

    FillInputVector(signal);
    FillFilterVector(filter);

    // Perform the convolution
    auto reference = Convolve1D(signal, filter, dsp::ConvolutionMethodOption::simple);
    auto result = Convolve1D(signal, filter, algorithm);

    // Compare results
    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, static_cast<ValueType>(epsilon)));
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
void TestConv2DVsSimple(int numRows, int numColumns, int numChannels, int filterSize, int numFilters, int stride, dsp::ConvolutionMethodOption algorithm)
{
    using Tensor = math::ChannelColumnRowTensor<ValueType>;

    const auto filterRows = filterSize;
    const auto filterColumns = filterSize;
    Tensor signal(numRows, numColumns, numChannels);
    Tensor filters(numFilters * filterRows, filterColumns, numChannels);

    FillInputTensor(signal);
    FillFiltersTensor(filters, numFilters);

    // Perform the convolution
    auto reference = Convolve2D(signal, filters, numFilters, stride, dsp::ConvolutionMethodOption::simple);
    auto result = Convolve2D(signal, filters, numFilters, stride, algorithm);

    // Compare results
    bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, static_cast<ValueType>(epsilon)));
    if (!ok)
    {
        std::cout << "Incorrect result for 2D tensor "
                  << " " << GetConvAlgName(algorithm) << " convolution on input of size " << signal.NumRows() << " x " << signal.NumColumns() << " x " << signal.NumChannels() << std::endl;

#if 0
        // Useful for pinpointing errors during debugging:
        std::cout << "Input:\n" << signal << std::endl;
        std::cout << std::endl;
        std::cout << "Filters:\n" << filters << std::endl;
        std::cout << std::endl;
        std::cout << "Reference:\n" << reference << std::endl;
        std::cout << std::endl;
        std::cout << "Computed:\n" << result << std::endl;
        std::cout << std::endl;
#endif
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
        std::cout << "Max difference:  " << *std::max_element(diffArray.begin(), diffArray.end()) << std::endl;
    }
}

// Depthwise-separable
template <typename ValueType>
void TestConv2DSeparableVsSimple(int numRows, int numColumns, int numChannels, int filterSize, int stride, dsp::ConvolutionMethodOption algorithm)
{
    using Tensor = math::ChannelColumnRowTensor<ValueType>;

    const int numFilters = numChannels;
    const auto filterRows = filterSize;
    const auto filterColumns = filterSize;
    Tensor signal(numRows, numColumns, numChannels);
    Tensor filters(numFilters * filterRows, filterColumns, 1);

    FillInputTensor(signal);
    FillFiltersTensor(filters, numFilters);

    // Perform the convolution
    auto fullResult = Convolve2DDepthwiseSeparable(signal, filters, numFilters, stride, algorithm);

    // Separately convolve each image / filter pair and compare with that channel of result
    for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
    {
        auto signalSlice = signal.GetSubTensor(0, 0, channelIndex, numRows, numColumns, 1);
        auto filterSlice = filters.GetSubTensor(channelIndex * filterRows, 0, 0, filterRows, filterColumns, 1);
        auto reference = Convolve2D(signalSlice, filterSlice, 1, stride, dsp::ConvolutionMethodOption::simple);
        auto result = fullResult.GetSubTensor(0, 0, channelIndex, fullResult.NumRows(), fullResult.NumColumns(), 1);

        // Compare results
        bool ok = testing::ProcessTest("Testing convolution result", reference.IsEqual(result, static_cast<ValueType>(epsilon)));
        if (!ok)
        {
            std::cout << "Incorrect result for channel " << channelIndex << " of 2D separable tensor "
                      << " " << GetConvAlgName(algorithm) << " convolution on input of size " << signal.NumRows() << " x " << signal.NumColumns() << " x " << signal.NumChannels() << std::endl;

#if 0
            // Useful for pinpointing errors during debugging:
            std::cout << "Input:\n" << signal << std::endl;
            std::cout << std::endl;
            std::cout << "Filter:\n" << filters << std::endl;
            std::cout << std::endl;
            std::cout << "Reference:\n" << reference << std::endl;
            std::cout << std::endl;
            std::cout << "Computed:\n" << result << std::endl;
            std::cout << std::endl;
#endif
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
            std::cout << "Max difference:  " << *std::max_element(diffArray.begin(), diffArray.end()) << std::endl;
        }
    }
}

//
// Explicit instantiations
//

// 1D
template void TestConv1D<float>(dsp::ConvolutionMethodOption);
template void TestConv1D<double>(dsp::ConvolutionMethodOption);
template void TestConv1DVsSimple<float>(int size, int filterSize, dsp::ConvolutionMethodOption algorithm);
template void TestConv1DVsSimple<double>(int size, int filterSize, dsp::ConvolutionMethodOption algorithm);

// 2D
template void TestConv2D<float>(dsp::ConvolutionMethodOption);
template void TestConv2D<double>(dsp::ConvolutionMethodOption);
template void TestConv2DVsSimple<float>(int numRows, int numColumns, int numChannels, int filterSize, int numFilters, int stride, dsp::ConvolutionMethodOption algorithm);
template void TestConv2DVsSimple<double>(int numRows, int numColumns, int numChannels, int filterSize, int numFilters, int stride, dsp::ConvolutionMethodOption algorithm);

// Depthwise-separable (i.e., multiple 2D in parallel)
template void TestConv2DSeparable<float>(dsp::ConvolutionMethodOption);
template void TestConv2DSeparable<double>(dsp::ConvolutionMethodOption);
template void TestConv2DSeparableVsSimple<float>(int numRows, int numColumns, int numChannels, int filterSize, int stride, dsp::ConvolutionMethodOption algorithm);
template void TestConv2DSeparableVsSimple<double>(int numRows, int numColumns, int numChannels, int filterSize, int stride, dsp::ConvolutionMethodOption algorithm);
