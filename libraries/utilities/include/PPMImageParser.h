////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PPMImageParser.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <istream>
#include <string>
#include <vector>

namespace ell
{
namespace utilities
{
    template <typename ValueType>
    struct Image
    {
        size_t width;
        size_t height;
        size_t numChannels;
        std::vector<ValueType> data;        
    };

    template <typename ValueType>
    Image<ValueType> ParsePPMStream(std::istream& in);

    template <typename ValueType>
    Image<ValueType> ParsePPMFile(const std::string& filename);
}
}
