////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GenerateTestModels.h (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dsp
#include "Convolution.h"

// model
#include "Map.h"

namespace ell
{
model::Map GenerateTreeModel(size_t numSplits);

// Neural nets
model::Map GenerateBinaryConvolutionModel(size_t imageRows, size_t imageColumns, size_t numChannels, size_t numFilters);
model::Map GenerateBinaryConvolutionPlusDenseModel(size_t imageRows, size_t imageColumns, size_t numChannels, size_t numFilters, size_t numOutputs);
model::Map GenerateBinaryDarknetLikeModel(bool lastLayerReal = false);
model::Map GenerateConvolutionModel(int inputRows, int inputColumns, int numChannels, int numFilters, int filterSize, int stride, dsp::ConvolutionMethodOption convolutionMethod);
} // namespace ell
