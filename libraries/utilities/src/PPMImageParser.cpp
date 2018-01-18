////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PPMImageParser.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PPMImageParser.h"
#include "Exception.h"
#include "Files.h"

// stl
#include <istream>

namespace ell
{
namespace utilities
{
    //
    // PPM format
    //
    //
    // P3  # channels
    // 28 28  # width, height
    // 65535  # max value
    // 38371 42017 29740 35984 43756 24554 32129 38649 21556 31277 37201 23208  # pixel values
    // ...

    template <typename ValueType>
    Image<ValueType> ParsePPMStream(std::istream& in)
    {
        // TODO: throw if we read bad values or hit EOF too soon
        Image<ValueType> result;
        char p;
        in >> p;
        if (p != 'P')
        {
            throw 0;
        }
        in >> result.numChannels;
        in >> result.width;
        in >> result.height;
        int maxValue = 0;
        in >> maxValue;
        auto numValues = result.numChannels * result.width * result.height;
        result.data = std::vector<ValueType>(numValues);
        auto numPixels = result.width * result.height;
        for (size_t index = 0; index < numValues; ++index)
        {
            int rawValue;
            in >> rawValue;
            ValueType value = rawValue / static_cast<ValueType>(maxValue);

            // convert from rgbrgbrgbrgbrgbrgb to rrrrggggbbbb
            int channel = static_cast<int>(index % result.numChannels);
            int outIndex = static_cast<int>(numPixels * channel + index / result.numChannels);
            result.data[outIndex] = value;
        }
        return result;
    }

    template <typename ValueType>
    Image<ValueType> ParsePPMFile(const std::string& filename)
    {
        auto inStream = OpenIfstream(filename);
        return ParsePPMStream<ValueType>(inStream);
    }

    template Image<int> ParsePPMStream(std::istream& in);
    template Image<int64_t> ParsePPMStream(std::istream& in);
    template Image<float> ParsePPMStream(std::istream& in);
    template Image<double> ParsePPMStream(std::istream& in);

    template Image<int> ParsePPMFile(const std::string& filename);
    template Image<int64_t> ParsePPMFile(const std::string& filename);
    template Image<float> ParsePPMFile(const std::string& filename);
    template Image<double> ParsePPMFile(const std::string& filename);
}
}
