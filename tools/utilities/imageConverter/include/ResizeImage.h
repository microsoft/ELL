////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ResizeImage.h (imageConverter)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

/// <summary> Resize the given .png or .jpg image to the given size and return the scaled RGB components as floating point numbers using the given scale. </summary>
template <typename T>
std::vector<T> ResizeImage(std::string& fileName, int rows, int cols, double inputScale);

