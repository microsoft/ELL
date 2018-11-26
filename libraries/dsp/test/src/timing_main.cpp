////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     timing_main.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionTiming.h"

#include <dsp/include/Convolution.h>

#include <testing/include/testing.h>

#include <utilities/include/Unused.h>

#include <iostream>

using namespace ell;
using namespace dsp;

struct ImageExtent
{
    int rows;
    int columns;
};

struct FiltersExtent
{
    int numFilters;
    int rows;
    int columns;
    int numChannels;
};

void TimeConvolutionImplementations(ImageExtent outputSize, FiltersExtent filterParams, ImageExtent stride, ImageExtent totalOutputPadding, int numIterations)
{
    UNUSED(stride, totalOutputPadding);

    const size_t filterSize = static_cast<size_t>(filterParams.rows);
    const size_t numChannels = static_cast<size_t>(filterParams.numChannels);
    const size_t numFilters = static_cast<size_t>(filterParams.numFilters);
    const size_t totalInputPadding = ((filterSize - 1) / 2) * 2;
    const size_t inputRows = static_cast<size_t>(outputSize.rows) + totalInputPadding;
    const size_t inputColumns = static_cast<size_t>(outputSize.columns) + totalInputPadding;
    TimeConv2D<float>(inputRows, inputColumns, numChannels, filterSize, numFilters, numIterations, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2D<float>(inputRows, inputColumns, numChannels, filterSize, numFilters, numIterations, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2D<float>(inputRows, inputColumns, numChannels, filterSize, numFilters, numIterations, ell::dsp::ConvolutionMethodOption::winograd);
}

int main()
{
    //
    // Timing
    //

    // 1D Convolution timing
    // void TimeConv1D(size_t signalSize, size_t filterSize, size_t numIterations, ell::dsp::ConvolutionMethodOption algorithm);
    TimeConv1D<float>(5000, 3, 1000, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv1D<float>(5000, 3, 1000, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";

    // 2D Convolution timing

    // void TimeConv2D(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, ell::dsp::ConvolutionMethodOption algorithm);
    TimeConv2D<float>(200, 200, 1, 3, 1, 100, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2D<float>(200, 200, 1, 3, 1, 100, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2D<float>(200, 200, 1, 3, 1, 1, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";

    TimeConv2D<float>(120, 80, 8, 3, 16, 1, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2D<float>(120, 80, 8, 3, 16, 1, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2D<float>(120, 80, 8, 3, 16, 1, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";

    TimeConv2D<float>(120, 80, 64, 3, 128, 1, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2D<float>(120, 80, 64, 3, 128, 1, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2D<float>(120, 80, 64, 3, 128, 1, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";

    TimeConv2D<float>(60, 40, 256, 3, 512, 1, ell::dsp::ConvolutionMethodOption::simple);
    TimeConv2D<float>(60, 40, 256, 3, 512, 1, ell::dsp::ConvolutionMethodOption::unrolled);
    TimeConv2D<float>(60, 40, 256, 3, 512, 1, ell::dsp::ConvolutionMethodOption::winograd);
    std::cout << "\n";

    int numIterations = 100;
    TimeConvolutionImplementations({ 16, 16 }, { 8, 3, 3, 8 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 16, 16 }, { 16, 3, 3, 16 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 16, 16 }, { 32, 3, 3, 32 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 16, 16 }, { 64, 3, 3, 64 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 16, 16 }, { 128, 3, 3, 128 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 16, 16 }, { 256, 3, 3, 256 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n\n";

    numIterations = 100;
    TimeConvolutionImplementations({ 31, 31 }, { 8, 3, 3, 8 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 31, 31 }, { 16, 3, 3, 16 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 31, 31 }, { 32, 3, 3, 32 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 31, 31 }, { 64, 3, 3, 64 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 31, 31 }, { 128, 3, 3, 128 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 31, 31 }, { 256, 3, 3, 256 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n\n";

    numIterations = 10;
    TimeConvolutionImplementations({ 64, 64 }, { 8, 3, 3, 8 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 64, 64 }, { 16, 3, 3, 16 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 64, 64 }, { 32, 3, 3, 32 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 64, 64 }, { 64, 3, 3, 64 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 64, 64 }, { 128, 3, 3, 128 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 64, 64 }, { 256, 3, 3, 256 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n\n";

    numIterations = 1;
    TimeConvolutionImplementations({ 127, 127 }, { 8, 3, 3, 8 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 127, 127 }, { 16, 3, 3, 16 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 127, 127 }, { 32, 3, 3, 32 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 127, 127 }, { 64, 3, 3, 64 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 127, 127 }, { 128, 3, 3, 128 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n";
    TimeConvolutionImplementations({ 127, 127 }, { 256, 3, 3, 256 }, { 1, 1 }, { 2, 2 }, numIterations);
    std::cout << "\n\n";

    return testing::DidTestFail() ? 1 : 0;
}
