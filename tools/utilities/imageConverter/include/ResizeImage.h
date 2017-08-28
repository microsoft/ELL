#pragma once

#include <string>
#include <vector>

///<summary>Resize the given .png or .jpg image to the given size and return the scaled RGB components as floating point numbers using the given scale.</summary>
std::vector<float> ResizeImage(std::string& fileName, int rows, int cols, float inputScale);

