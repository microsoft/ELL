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
    class NullStreamBuf : public std::streambuf
    {
        virtual int overflow(int c)
        {
            return std::char_traits<char>::not_eof(c) ? c : EOF; 
        }
    };

    NullStreamBuf nullStreamBuf;

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
        else if(filenameOrEmpty == "null")
        {
            _outBuf = &nullStreamBuf;
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
