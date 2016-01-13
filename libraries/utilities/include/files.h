// files.h

#pragma once

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <string>
using std::string;

namespace utilities
{
    /// Opens an ifstream and throws an exception if a problem occurs
    ///
    ifstream OpenIfstream(string filepath);
    
    /// Opens an ofstream and throws an exception if a problem occurs
    ///
    ofstream OpenOfstream(string filepath);

    /// \returns True if the file exists and can be opened
    ///
    bool FileIsReadable(string filepath);
}