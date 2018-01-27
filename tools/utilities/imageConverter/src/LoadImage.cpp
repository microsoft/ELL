////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadImage.cpp (imageConverter)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InvokePython.h"
#include "LoadImage.h"
#include <algorithm>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <vector>

template <typename T>
std::vector<T> LoadImage(std::string& fileName, int width, int height, double inputScale, PixelOrder order)
{
    std::stringstream stream;
    stream << R"xx(
import sys
import numpy as np
import cv2

def resize_image(image, newSize) :
    if (image.shape[0] > image.shape[1]) : # Tall(more rows than cols)
        rowStart = int((image.shape[0] - image.shape[1]) / 2)
        rowEnd = rowStart + image.shape[1]
        colStart = 0
        colEnd = image.shape[1]
    else: # Wide(more cols than rows)
        rowStart = 0
        rowEnd = image.shape[0]
        colStart = int((image.shape[1] - image.shape[0]) / 2)
        colEnd = colStart + image.shape[0]

        cropped = image[rowStart:rowEnd, colStart : colEnd]
        resized = cv2.resize(cropped, newSize)
        return resized

def prepare_image_for_predictor(image, newSize, scale, bgr2rgb) :
    resized = resize_image(image, newSize)
    if bgr2rgb != 0:
        resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
    resized = resized.astype(np.float).ravel()
    resized = resized * scale
    return resized

def save_raw(name, data) :
    with open(name, 'wb') as f :
        f.write(bytearray(data))
        f.close()

def main() :
    file = sys.argv[1]
    rows = int(sys.argv[2])
    cols = int(sys.argv[3])
    scale = 1 / 255
    bgr2rgb = 1
    if len(sys.argv) >= 5:
        scale = float(sys.argv[4])
    if len(sys.argv) >= 6:
        bgr2rgb = int(sys.argv[5])

    image = cv2.imread(file)
    if image is None:
        print("Error reading image {}".format(file))
    resized = prepare_image_for_predictor(image, (rows, cols), scale, bgr2rgb)
    save_raw(file + '.dat', resized)

main()

)xx";

    ExecutePythonScript(stream.str(), { "", fileName, std::to_string(height), std::to_string(width), std::to_string(inputScale), std::to_string((int)order) });

    // now load the result.
    std::ifstream stm(fileName + ".dat", std::ios::in | std::ios::binary);
    stm.seekg(0, std::ios::end);
    size_t size = stm.tellg();
    stm.seekg(0);
    size_t len = size / sizeof(double);
    auto buffer = std::vector<double>(len);
    stm.read((char*)buffer.data(), size);
    std::vector<T> result(len);
    for (size_t i = 0; i < len; i++)
    {
        result[i] = static_cast<T>(buffer[i]);
    }
    return result;
}

template std::vector<int> LoadImage(std::string& fileName, int width, int height, double inputScale, PixelOrder order);

template std::vector<int64_t> LoadImage(std::string& fileName, int width, int height, double inputScale, PixelOrder order);

template std::vector<float> LoadImage(std::string& fileName, int width, int height, double inputScale, PixelOrder order);

template std::vector<double> LoadImage(std::string& fileName, int width, int height, double inputScale, PixelOrder order);
