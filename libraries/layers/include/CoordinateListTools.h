////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CoordinateListTools.h (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoordinateList.h"

// stl
#include <cstdint>
#include <string>
#include <memory>

namespace layers
{
    class Model;

    /// <summary>
    /// Builds a coordinate list from a coordinate list definition string. The string has the following format
    /// [defString] .=. [def];[defString]
    /// [def] .=. [layerIndex] || [layerIndex],[elementIndex] || [layerIndex],[elementInterval]
    /// [elementInterval] .=. [fromElementIndex]:[toElementIndex]   (used to define intervals of elements)
    /// [layerIndex] .=. [index]
    /// [elementIndex] .=. [index]
    /// [fromElementIndex] .=. [index]
    /// [toElementIndex] .=. [index]
    /// [index] .=. (uint) || e || e-(uint)                         ("e" gets translated to the maximal index possible in the map) 
    /// </summary>
    ///
    /// <param name="model"> The model which the coordinate list relates to. </param>
    /// <param name="inputLayerSize"> The dataset dimension </param>
    /// <param name="coordinateListString"> The coordinate list definition string. </param>
    ///
    /// <returns> The coordinate list. </returns>
    layers::CoordinateList BuildCoordinateList(const layers::Model& model, uint64_t inputLayerSize, const std::string& coordinateListString);
}

