////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputStreamImpostor.cpp (utilities)
//  Authors:  Chuck Jacobs
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

    OutputStreamImpostor GetOutputStreamImpostor(std::string filenameOrEmpty)
    {
        return OutputStreamImpostor(filenameOrEmpty);
    }
}
