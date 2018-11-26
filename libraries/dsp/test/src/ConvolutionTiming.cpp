////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTiming.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionTiming.h"
#include "ConvolutionTestData.h"
#include "DSPTestUtilities.h"

#include <dsp/include/Convolution.h>
#include <dsp/include/WinogradConvolution.h>

#include <math/include/MathConstants.h>
#include <math/include/Tensor.h>
#include <math/include/TensorOperations.h>
#include <math/include/Vector.h>
#include <math/include/VectorOperations.h>

#include <testing/include/testing.h>

#include <utilities/include/MillisecondTimer.h>

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace ell;

//
// Timing
//
template <typename ValueType>
void TimeConv1D(size_t signalSize, size_t filterSize, size_t numIterations, dsp::ConvolutionMethodOption algorithm)
{
    math::RowVector<ValueType> signal(signalSize);
    math::RowVector<ValueType> filter(filterSize);

    // Perform the convolution
    utilities::MillisecondTimer timer;
    for (size_t iter = 0; iter < numIterations; ++iter)
    {
        volatile auto result = Convolve1D(signal, filter, algorithm);
    }
    auto duration = timer.Elapsed();

    std::cout << "Time to perform 1D "
              << " " << GetConvAlgName(algorithm) << " convolution on size-" << signalSize << " input and size-" << filterSize << " filter: " << duration << " ms" << std::endl;
}

template <typename ValueType>
void TimeConv2D(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, dsp::ConvolutionMethodOption algorithm)
{
    const auto filterRows = filterSize;
    const auto filterColumns = filterSize;
    math::ChannelColumnRowTensor<ValueType> signal(numRows, numColumns, numChannels);
    math::ChannelColumnRowTensor<ValueType> filters{ numFilters * filterRows, filterColumns, numChannels };

    // Perform the convolution
    utilities::MillisecondTimer timer;
    if (algorithm == dsp::ConvolutionMethodOption::winograd)
    {
        const auto order = dsp::WinogradFilterOrder::tilesFirst;
        const int tileSize = 2;
        auto transformedFilters = dsp::GetTransformedFilters(filters, static_cast<int>(numFilters), tileSize, order);
        timer.Reset();
        for (size_t iter = 0; iter < numIterations; ++iter)
        {
            volatile auto result = Convolve2DWinogradPretransformed(signal, transformedFilters, static_cast<int>(numFilters), tileSize, static_cast<int>(filterSize), order);
        }
    }
    else
    {
        for (size_t iter = 0; iter < numIterations; ++iter)
        {
            volatile auto result = Convolve2D(signal, filters, static_cast<int>(numFilters), algorithm);
        }
    }
    auto duration = timer.Elapsed();

    std::cout << "Time to perform 2D " << GetConvAlgName(algorithm) << " tensor convolution on " << GetSizeString(signal) << " input with " << GetFilterSizeString(filters) << " filters: " << duration << " ms" << std::endl;
}

//
// Explicit instantiations
//

// 1D
template void TimeConv1D<float>(size_t signalSize, size_t filterSize, size_t numIterations, dsp::ConvolutionMethodOption);
template void TimeConv1D<double>(size_t signalSize, size_t filterSize, size_t numIterations, dsp::ConvolutionMethodOption);

// 2D (Tensor)
template void TimeConv2D<float>(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, dsp::ConvolutionMethodOption algorithm);
template void TimeConv2D<double>(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, dsp::ConvolutionMethodOption algorithm);
