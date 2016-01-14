// files.h

#pragma once

// stl
#include <fstream>
using std::ifstream;
using std::ofstream;

#include <string>
using std::string;

#include <memory>
using std::unique_ptr;

namespace utilities
{
    /// Opens an ifstream and throws an exception if a problem occurs
    ///
    unique_ptr<ifstream> OpenIfstream(string filepath); // returns ptr because ifstream has deleted copy/move ctors
    
    /// Opens an ofstream and throws an exception if a problem occurs
    ///
    unique_ptr<ofstream> OpenOfstream(string filepath);

    /// \returns True if the file exists and can be opened
    ///
    bool FileIsReadable(string filepath);
}