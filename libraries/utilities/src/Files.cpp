////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     files.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Files.h"

// utiliites
#include "Exception.h"

// stl
#include <stdexcept>
#include <memory>

namespace utilities
{
    std::ifstream OpenIfstream(std::string filepath)
    {
        // open file
        auto fs = std::ifstream(filepath);

        // check that it opened
        if(!fs.is_open())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "error openning file " + filepath);
        }

        return fs;
    }

    std::ofstream OpenOfstream(std::string filepath)
    {
        // open file
        auto fs = std::ofstream(filepath);

        // check that it opened
        if (!fs.is_open())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "error openning file " + filepath);
        }

        return fs;
    }

    bool IsFileReadable(std::string filepath)
    {
        // open file
        std::ifstream fs(filepath);

        // check that it opened
        if(fs.is_open())
        {
            return true;
        }

        return false;
    }
}
