////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTiming.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dsp
#include "Convolution.h"

// 1D convolution over a vector
template <typename ValueType>
void TimeConv1D(size_t signalSize, size_t filterSize, size_t numIterations, ell::dsp::ConvolutionMethodOption algorithm);

// 2D convolution over a tensor
template <typename ValueType>
void TimeConv2D(size_t numRows, size_t numColumns, size_t numChannels, size_t filterSize, size_t numFilters, size_t numIterations, ell::dsp::ConvolutionMethodOption algorithm);
