////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TextLine.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CStringParser.h"
#include "Exception.h"

namespace ell
{
namespace data
{
    template <typename ValueType>
    void TextLine::ParseAdvance(ValueType& value)
    {
        auto result = utilities::Parse(_currentChar, value);
        if (result != utilities::ParseResult::success)
        {
            throw utilities::DataFormatException(utilities::DataFormatErrors::badFormat, "could not parse value");
        }
    }

    template <typename ValueType>
    size_t TextLine::TryParse(ValueType& value) const
    {
        auto temp = _currentChar;
        auto result = utilities::Parse(temp, value);
        if (result == utilities::ParseResult::success)
        {
            auto stepSize = static_cast<size_t>(temp - _currentChar);
            return stepSize;
        }
        else
        {
            return 0;
        }
    }
}
}