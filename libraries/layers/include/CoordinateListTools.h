////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CoordinateListTools.h (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Coordinate.h"

// stl
#include <string>

namespace layers
{
    class Stack;

    /// <summary>
    /// Gets a coordinate list from a coordinate list definition string. The string has the following format
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
    /// <param name="stack"> The stack which the coordinate list relates to. </param>
    /// <param name="coordinateListString"> The coordinate list definition string. </param>
    ///
    /// <returns> The coordinate list. </returns>
    layers::CoordinateList GetCoordinateList(const layers::Stack& stack, const std::string& coordinateListString);
}

