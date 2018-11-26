////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionTest.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <dsp/include/Convolution.h>

struct Extent2D
{
    int numRows;
    int numColumns;
};

struct FilterSize
{
    int numFilters;
    int filterRows;
    int filterColumns;
    int numChannels;
};

// 1D convolution over a vector
template <typename ValueType>
void TestConv1D(ell::dsp::ConvolutionMethodOption algorithm);

template <typename ValueType>
void TestConv1DVsSimple(int size, int filterSize, ell::dsp::ConvolutionMethodOption algorithm);

// 2D convolution over a tensor
template <typename ValueType>
void TestConv2D(ell::dsp::ConvolutionMethodOption algorithm);

template <typename ValueType>
void TestConv2DVsSimple(int numRows, int numColumns, int numChannels, int filterSize, int numFilters, int stride, ell::dsp::ConvolutionMethodOption algorithm);

// Depthwise-separable 2D (multiple "flat" 2D in parallel)
template <typename ValueType>
void TestConv2DSeparable(ell::dsp::ConvolutionMethodOption algorithm);

template <typename ValueType>
void TestConv2DSeparableVsSimple(int numRows, int numColumns, int numChannels, int filterSize, int stride, ell::dsp::ConvolutionMethodOption algorithm);
