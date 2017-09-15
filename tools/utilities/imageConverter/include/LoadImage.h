////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadImage.h (imageConverter)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

enum class PixelOrder {
	BGR,
	RGB
};

/// <summary>Load and resize the given .png or .jpg image to the given size and return the scaled RGB components as a vector of floating point numbers scaled by the given scale.
/// The PixelOrder defines how you want the image returned, either BGR (blue, green, red) or RGB (red, green, blue) order.
/// </summary>
template <typename T>
std::vector<T> LoadImage(std::string& fileName, int width, int height, double inputScale, PixelOrder order);
