////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     Files.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <fstream>
#include <string>

namespace utilities
{
    /// <summary> Opens an std::ifstream and throws an exception if a problem occurs. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The stream. </returns>
    std::ifstream OpenIfstream(std::string filepath);

    /// <summary> Opens an std::ofstream and throws an exception if a problem occurs. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The stream. </returns>
    std::ofstream OpenOfstream(std::string filepath);

    /// <summary> Returns true if the file exists and can be opened. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> true if the file exists and is readable. </returns>
    bool IsFileReadable(std::string filepath);
}
