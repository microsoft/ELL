/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NodesTestUtilities.h (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dsp
#include "WinogradConvolution.h"

struct ImageShape
{
    int numRows;
    int numColumns;
    int numChannels;
};

struct FiltersShape
{
    int numFilters;
    int numRows;
    int numColumns;
    int numChannels; // 0 means "match number of channels of input"
};

// Parameter struct for passing options in to Winograd convolution tests.
struct WinogradOptions
{
    int tileSize;
    ell::dsp::WinogradFilterOrder filterOrder;
};

struct SimpleOptions
{
};

struct UnrolledOptions
{
};

struct DiagonalOptions
{
};

union ConvolutionOptions
{
    ConvolutionOptions() {}
    ConvolutionOptions(int tileSize, ell::dsp::WinogradFilterOrder order) :
        winogradOptions({ tileSize, order }) {}
    ConvolutionOptions(int tileSize) :
        winogradOptions({ tileSize, ell::dsp::WinogradFilterOrder::tilesFirst }) {}

    WinogradOptions winogradOptions;
    SimpleOptions simpleOptions;
    UnrolledOptions unrolledOptions;
    DiagonalOptions diagonalOptions;
};
