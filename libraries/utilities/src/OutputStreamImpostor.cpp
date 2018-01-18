////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputStreamImpostor.cpp (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel, Kern Handa
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
    static std::ofstream nullStreamBuf;

    OutputStreamImpostor::OutputStreamImpostor() : _outputStream(nullStreamBuf) {}

    OutputStreamImpostor::OutputStreamImpostor(StreamType streamType) : OutputStreamImpostor()
    {
        switch (streamType)
        {
        case StreamType::cout:
            _outputStream = std::cout;
            break;
        case StreamType::cerr:
            _outputStream = std::cerr;
            break;
        default:
            break; // delegated default ctor already sets it to nullStreamBuf
        }
    }

    OutputStreamImpostor::OutputStreamImpostor(std::ostream& stream) : _outputStream(stream) {}

    OutputStreamImpostor::OutputStreamImpostor(const std::string& filename) :
        _fileStream(std::make_shared<std::ofstream>(OpenOfstream(filename))),
        _outputStream(*_fileStream)
    {}
}
}
