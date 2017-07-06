////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Files.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <fstream>
#include <string>

namespace ell
{
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

    /// <summary> Returns true if the file exists and can be for reading. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> true if the file exists and is readable. </returns>
    bool IsFileReadable(std::string filepath);

    /// <summary> Returns true if the file exists and can be for writing. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> true if the file exists and is readable. </returns>
    bool IsFileWritable(std::string filepath);

    /// <summary> Returns the file extension, optionally converted to lower-case. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The file extension, not including the ".". </returns>
    std::string GetFileExtension(std::string filepath, bool toLowercase = false);

    /// <summary> Returns the file path minus extension. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The filepath with extension. </returns>
    std::string RemoveFileExtension(std::string filepath);

    /// <summary> Returns the filename from a path. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The filename. </returns>
    std::string GetFileName(std::string filepath);

    /// <summary> Returns the filename from a path. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The filename. </returns>
    std::string JoinPaths(std::string path1, std::string path2);
}
}
