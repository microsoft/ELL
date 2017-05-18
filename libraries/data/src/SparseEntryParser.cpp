////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SparseEntryParser.cpp (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SparseEntryParser.h"

// utilities
#include "Exception.h"
#include "Parser.h"

// stl
#include <memory>
#include <stdexcept>

namespace ell
{
namespace data
{
    std::string getSnippet(const char* pos)
    {
        std::string str;
        const char* end = pos + 20;
        while (*pos != '\0' && pos < end)
        {
            str.push_back(*pos);
            ++pos;
        }
        return str;
    }

    void SparseEntryParser::Iterator::Next()
    {
        // parse index
        size_t index;
        auto result = utilities::Parse(_currentPos, index);

        // handle errors
        if (result != utilities::ParseResult::success)
        {
            if (result == utilities::ParseResult::endOfString || result == utilities::ParseResult::beginComment)
            {
                _isValid = false;
                return;
            }
            if (result == utilities::ParseResult::badFormat)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "expected unsigned integer near '... " + getSnippet(_currentPos) + " ...' in '" + *_pExampleString + "'");
            }
            else if (result == utilities::ParseResult::outOfRange)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "out of unsigned integer range near '... " + getSnippet(_currentPos) + " ...' in '" + *_pExampleString + "'");
            }
        }

        // expect ':' character between index and value
        if (*_currentPos != ':')
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "expected ':' between index and value near '" + getSnippet(_currentPos) + " ...' in '" + *_pExampleString + "'");
        }
        ++_currentPos;

        double value;
        result = utilities::Parse(_currentPos, value);

        if (result != utilities::ParseResult::success)
        {
            if (result == utilities::ParseResult::endOfString || result == utilities::ParseResult::beginComment)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "string ended prematurely in " + *_pExampleString);
            }
            if (result == utilities::ParseResult::badFormat)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "expected double near '... " + getSnippet(_currentPos) + " ...' in '" + *_pExampleString + "'");
            }
            else if (result == utilities::ParseResult::outOfRange)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "out of double range near '... " + getSnippet(_currentPos) + " ...' in '" + *_pExampleString + "'");
            }
        }

        _currentIndexValue = IndexValue{ index, value };
    }

    SparseEntryParser::Iterator::Iterator(std::shared_ptr<const std::string> pExampleString, const char* pStr)
        : _pExampleString(pExampleString), _currentPos(pStr)
    {
        Next();
    }

    SparseEntryParser::Iterator SparseEntryParser::GetIterator(std::shared_ptr<const std::string> pExampleString, const char* pStr) const
    {
        return Iterator(pExampleString, pStr);
    }
}
}
