////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SequentialLineIterator.cpp (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SequentialLineIterator.h"

// utilities
#include "Files.h"

namespace ell
{
namespace data
{
    SequentialLineIterator::SequentialLineIterator(std::istream& stream, char delim)
        : _stream(stream), _delim(delim)
    {
        Next();
    }

    void SequentialLineIterator::Next()
    {
        std::string nextLine;
        std::getline(_stream, nextLine, _delim);

        if (_stream.fail())
        {
            _isValid = false;
            return;
        }

        _currentLine = TextLine(std::move(nextLine));
    }
}
}
