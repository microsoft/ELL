// files.cpp

#include "files.h"

#include <stdexcept>
using std::runtime_error;

#include <memory>
using std::make_unique;

namespace utilities
{
    unique_ptr<ifstream> OpenIfstream(string filepath)
    {
        // open file
        auto fs = make_unique<ifstream>(filepath);

        // check that it opened
        if(!fs->is_open())
        {
            throw runtime_error("error openning file " + filepath);
        }

        return fs;
    }

    unique_ptr<ofstream> OpenOfstream(string filepath)
    {
        // open file
        auto fs = make_unique<ofstream>(filepath);

        // check that it opened
        if (!fs->is_open())
        {
            throw runtime_error("error openning file " + filepath);
        }

        return fs;
    }

    bool FileIsReadable(string filepath)
    {
        // open file
        ifstream fs(filepath);

        // check that it opened
        if(!fs.is_open())
        {
            return true;
        }

        return false;
    }
}