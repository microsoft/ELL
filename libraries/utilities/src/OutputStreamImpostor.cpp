////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputStreamImpostor.cpp (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputStreamImpostor.h"

// utilities
#include "Files.h"

// stl
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

namespace ell
{
namespace utilities
{
    class NullStreamBuf : public std::streambuf
    {
        virtual int overflow(int c)
        {
            return std::char_traits<char>::not_eof(c) ? c : EOF;
        }
    };

    NullStreamBuf nullStreamBuf;

    OutputStreamImpostor::OutputStreamImpostor(StreamType streamType)
    {
        if (streamType == StreamType::cout)
        {
            _outputStream = std::make_shared<std::ostream>(std::cout.rdbuf());
        }
        else if (streamType == StreamType::cerr)
        {
            _outputStream = std::make_shared<std::ostream>(std::cerr.rdbuf());
        }
        else // null
        {
            _outputStream = std::make_shared<std::ostream>(&nullStreamBuf);
        }
    }

    OutputStreamImpostor::OutputStreamImpostor(std::string filename)
    {
        _outputFileStream = std::make_shared<std::ofstream>(utilities::OpenOfstream(filename));
        _outputStream = std::make_shared<std::ostream>(_outputFileStream->rdbuf());
    }
}
}
