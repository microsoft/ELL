////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     files.h (utilities)
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
    /// Opens an std::ifstream and throws an exception if a problem occurs
    ///
    std::ifstream OpenIfstream(std::string filepath);
    
    /// Opens an std::ofstream and throws an exception if a problem occurs
    ///
    std::ofstream OpenOfstream(std::string filepath);

    /// \returns True if the file exists and can be opened
    ///
    bool IsFileReadable(std::string filepath);
}
