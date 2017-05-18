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
    SequentialLineIterator::SequentialLineIterator(const std::string& filepath, char delim)
        : _delim(delim)
    {
        _iFStream = utilities::OpenIfstream(filepath);
        Next();
    }

    void SequentialLineIterator::Next()
    {
        auto pNextLine = std::make_shared<std::string>();
        std::getline(_iFStream, *pNextLine, _delim);
        if (_iFStream.fail())
        {
            _pCurrentLine = nullptr;
        }
        else
        {
            _pCurrentLine = pNextLine;
        }
    }
}
}
