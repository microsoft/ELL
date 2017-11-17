////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GenerateTestModels.h (profile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "DynamicMap.h"

namespace ell
{
    model::DynamicMap GenerateTreeModel(size_t numSplits);

    // Neural nets
    model::DynamicMap GenerateBinaryConvolutionModel(size_t imageRows, size_t imageColumns, size_t numChannels, size_t numFilters);
    model::DynamicMap GenerateBinaryConvolutionPlusDenseModel(size_t imageRows, size_t imageColumns, size_t numChannels, size_t numFilters, size_t numOutputs);
    model::DynamicMap GenerateBinaryDarknetLikeModel(bool lastLayerReal=false);
}
