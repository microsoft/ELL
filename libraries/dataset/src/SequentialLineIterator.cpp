////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     SequentialLineIterator.cpp (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SequentialLineIterator.h"

// utilities
#include "Files.h"

namespace dataset
{
    SequentialLineIterator::SequentialLineIterator(const std::string& filepath, char delim) : _delim(delim)
    {
        _iFStream = utilities::OpenIfstream(filepath);
        Next();
    }

    bool SequentialLineIterator::IsValid() const
    {
        return (_spCurrentLine != nullptr);
    }

    void SequentialLineIterator::Next()
    {
        auto spNextLine = std::make_shared<std::string>();
        std::getline(_iFStream, *spNextLine, _delim);
        if(_iFStream.fail())
        {
            _spCurrentLine = nullptr;
        }
        else
        {
            _spCurrentLine = spNextLine;
        }
    }

    std::shared_ptr<const std::string> SequentialLineIterator::Get() const
    {
        return _spCurrentLine;
    }
}
