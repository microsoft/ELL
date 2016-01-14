// files.cpp

#include "files.h"

#include <stdexcept>
using std::runtime_error;

#include <memory>
using std::make_unique;

namespace utilities
{
    ifstream OpenIfstream(string filepath)
    {
        // open file
        auto fs = ifstream(filepath);

        // check that it opened
        if(!fs.is_open())
        {
            throw runtime_error("error openning file " + filepath);
        }

        return fs;
    }

    ofstream OpenOfstream(string filepath)
    {
        // open file
        auto fs = ofstream(filepath);

        // check that it opened
        if (!fs.is_open())
        {
            throw runtime_error("error openning file " + filepath);
        }

        return fs;
    }

    bool IsFileReadable(string filepath)
    {
        // open file
        ifstream fs(filepath);

        // check that it opened
        if(fs.is_open())
        {
            return true;
        }

        return false;
    }
}