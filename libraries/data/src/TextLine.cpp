////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TextLine.cpp (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TextLine.h"

// utilities
#include "CStringParser.h"

namespace ell
{
namespace data
{
    TextLine::TextLine(std::string string) : _string(std::make_shared<const std::string>(std::move(string))), _currentChar(_string->c_str())
    {
    }

    char TextLine::Peek(size_t increment) const 
    { 
        return *(_currentChar + increment); 
    }

    bool TextLine::IsEndOfContent() const
    {
        if ((*_currentChar == '\0') || (*_currentChar == '#') || (*_currentChar == '/' && *(_currentChar + 1) == '/'))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    size_t TextLine::GetCurrentPosition() const
    {
        return static_cast<size_t>(_currentChar - _string->c_str());
    }

    void TextLine::AdvancePosition(size_t increment)
    {
        _currentChar += increment;
    }

}
}