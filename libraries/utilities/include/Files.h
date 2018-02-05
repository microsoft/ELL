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
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> Opens an std::ifstream and throws an exception if a problem occurs. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The stream. </returns>
    std::ifstream OpenIfstream(const std::string& filepath);

    /// <summary> Opens an std::ofstream and throws an exception if a problem occurs. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The stream. </returns>
    std::ofstream OpenOfstream(const std::string& filepath);

    /// <summary> Returns true if the file exists and can be opened for reading. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> true if the file exists and is readable. </returns>
    bool IsFileReadable(const std::string& filepath);

    /// <summary> Returns true if the file exists and can be opened for writing. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> true if the file exists and is readable. </returns>
    bool IsFileWritable(const std::string& filepath);


    /// <summary> Use this method to check if file exists. This might be necessary instead of
    /// IsFileReadable in cases where you know file is a locked executable and you just want
    /// to know if it exists and you don't intend to open it.
    /// </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> true if the file exists. </returns>
    bool FileExists(const std::string& filepath);

    /// <summary> Returns the file extension, optionally converted to lower-case. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The file extension, not including the ".". </returns>
    std::string GetFileExtension(const std::string& filepath, bool toLowercase = false);

    /// <summary> Returns the file path minus extension. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The filepath with extension. </returns>
    std::string RemoveFileExtension(const std::string& filepath);

    /// <summary> Returns the filename from a path. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The filename. </returns>
    std::string GetFileName(const std::string& filepath);

    /// <summary> Returns the directory name minus the file name from a path. </summary>
    ///
    /// <param name="filepath"> The path. </param>
    ///
    /// <returns> The path to the file. </returns>
    std::string GetDirectoryPath(const std::string& filepath);

    /// <summary> Returns true if the given directory exists. </summary>
    ///
    /// <param name="path"> The path. </param>
    ///
    /// <returns> The path to the directory. </returns>
    bool DirectoryExists(const std::string& path);

    /// <summary> Returns true if the given directory exists or was created. </summary>
    ///
    /// <param name="path"> The path. </param>
    ///
    /// <returns> The path to the directory. </returns>
    void EnsureDirectoryExists(const std::string& path);

    /// <summary> Returns the combined filename from joining two or more paths. </summary>
    ///
    /// <param name="path"> The starting path. </param>
    ///
    /// <returns> The combined filename. </returns>
    std::string JoinPaths(const std::string& path1, const std::string& path2);

    /// <summary> Returns the combined filename from joining two or more paths. </summary>
    ///
    /// <param name="path"> The starting path. </param>
    /// <param name="toAdd"> The paths to append. </param>
    ///
    /// <returns> The combined filename. </returns>
    std::string JoinPaths(const std::string& path, std::initializer_list<std::string> toAdd);

    /// <summary> Returns the combined filename from joining two or more paths. </summary>
    ///
    /// <param name="path"> The starting path. </param>
    /// <param name="toAdd"> The paths to append. </param>
    ///
    /// <returns> The combined filename. </returns>
    std::string JoinPaths(const std::string& path, std::vector<std::string> toAdd);


    /// <summary> Split a file path into it's parts using OS specific path separator. </summary>
    ///
    /// <param name="paths"> The path. </param>
    ///
    /// <returns> The parts of the path. </returns>
    std::vector<std::string> SplitPath(const std::string& path);

    /// <summary> Returns the current working directory. </summary>
    ///
    /// <returns> The path. </returns>
    std::string GetWorkingDirectory();

    /// <summary> Find a program using the current user PATH environment. </summary>
    ///
    /// <param name="name"> The name of the executable to find. </param>
    /// <returns> The full path to the executable if found or empty string. </returns>
    std::string FindExecutable(const std::string& name);
}
}
