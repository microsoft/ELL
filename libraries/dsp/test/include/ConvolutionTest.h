////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTest.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Convolution.h"

// 1D convolution over a vector
template <typename ValueType>
void TestConv1DVsSimple(size_t size, size_t filterSize, ell::dsp::ConvolutionMethodOption algorithm);

template <typename ValueType>
void TestConv1D(ell::dsp::ConvolutionMethodOption algorithm);

template <typename ValueType>
void TimeConv1D(size_t signalSize, size_t numIterations, ell::dsp::ConvolutionMethodOption algorithm);

// 2D convolution over a matrix
template <typename ValueType>
void TestConv2DMatrixVsSimple(size_t numRows, size_t numColumns, size_t filterSize, ell::dsp::ConvolutionMethodOption algorithm);

template <typename ValueType>
void TestConv2D(ell::dsp::ConvolutionMethodOption algorithm);

template <typename ValueType>
void TimeConv2D(size_t numRows, size_t numColumns, size_t numIterations, ell::dsp::ConvolutionMethodOption algorithm);

// 2D convolution over a tensor
template <typename ValueType>
void TestConv2DTensorVsSimple(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, ell::dsp::ConvolutionMethodOption algorithm);

template <typename ValueType>
void TimeConv2DTensor(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, ell::dsp::ConvolutionMethodOption algorithm);
