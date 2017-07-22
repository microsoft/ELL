#pragma once

#include <string>
#include <vector>

///<summary>Resize the given .png or .jpg image to the given size and return the scaled RGB components as floating point numbers using scale of 1/255.</summary>
std::vector<float> ResizeImage(std::string& fileName, int rows, int cols);

