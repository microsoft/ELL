////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     OutputStreamImpostor.cpp (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputStreamImpostor.h"

// utilities
#include "Files.h"

// stl
#include <string>
#include <ostream>
#include <fstream>
#include <memory>
#include <iostream>

namespace utilities
{
    OutputStreamImpostor::OutputStreamImpostor()
    {
        _outBuf = std::cout.rdbuf();
        _out = std::make_unique<std::ostream>(_outBuf);
    }

    OutputStreamImpostor::OutputStreamImpostor(std::string filenameOrEmpty)
    {
        if (filenameOrEmpty == "")
        {
            _outBuf = std::cout.rdbuf();
        }
        else
        {
            _outputFileStream = OpenOfstream(filenameOrEmpty);
            _outBuf = _outputFileStream.rdbuf();
        }
        _out = std::make_unique<std::ostream>(_outBuf);
    }

    OutputStreamImpostor::operator std::ostream&() &
    {
        return *_out;
    }

    OutputStreamImpostor::operator std::ostream const&() const &
    {
        return *_out;
    }

    OutputStreamImpostor GetOutputStreamImpostor(std::string filenameOrEmpty)
    {
        return OutputStreamImpostor(filenameOrEmpty);
    }
}
