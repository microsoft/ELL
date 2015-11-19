// SparseEntryParser.cpp

#include "SparseEntryParser.h"
#include "Parser.h"

#include <memory>
using std::move;

#include <stdexcept>
using std::runtime_error;

namespace dataset
{
    bool SparseEntryParser::Iterator::IsValid() const
    {
        return _isValid;
    }

    string getSnippet(const char* pos)
    {
        string str;
        const char* end = pos + 20;
        while(*pos != '\0' && pos < end)
        {
            str.push_back(*pos);
            ++pos;
        }
        return str;
    }

    void SparseEntryParser::Iterator::Next()
    {
        // parse index
        uint64 index;
        auto result = Parser::Parse(_currentPos, index);

        // handle errors
        if(result != Parser::Result::success)
        {
            if(result == Parser::Result::endOfString || result == Parser::Result::beginComment)
            {
                _isValid = false;
                return;
            }
            if(result == Parser::Result::badFormat)
            {
                throw runtime_error("bad format, expected unsigned integer near '... " + getSnippet(_currentPos) + " ...' in '" + *_spExampleString + "'");
            }
            else if(result == Parser::Result::outOfRange)
            {
                throw runtime_error("index out of unsigned integer range near '... " + getSnippet(_currentPos) + " ...' in '" + *_spExampleString + "'");
            }
        }

        // expect ':' character between index and value
        if(*_currentPos != ':')
        {
            throw runtime_error("bad format, expected ':' between index and value near '" + getSnippet(_currentPos) + " ...' in '" + *_spExampleString + "'");
        }
        ++_currentPos;

        double value;
        result = Parser::Parse(_currentPos, value);

        if(result != Parser::Result::success)
        {
            if(result == Parser::Result::endOfString || result == Parser::Result::beginComment)
            {
                throw runtime_error("bad format, string ended prematurely in " + *_spExampleString);
            }
            if(result == Parser::Result::badFormat)
            {
                throw runtime_error("bad format, expected double near '... " + getSnippet(_currentPos) + " ...' in '" + *_spExampleString + "'");
            }
            else if(result == Parser::Result::outOfRange)
            {
                throw runtime_error("index out of double range near '... " + getSnippet(_currentPos) + " ...' in '" + *_spExampleString + "'");
            }
        }

        _currentIndexValue = indexValue{index, value};
    }

    indexValue SparseEntryParser::Iterator::GetValue() const
    {
        return _currentIndexValue;
    }

    SparseEntryParser::Iterator::Iterator(shared_ptr<const string> spExampleString, const char* pStr) : _spExampleString(spExampleString), _currentPos(pStr)
    {
        Next();
    }

    SparseEntryParser::Iterator SparseEntryParser::GetIterator(shared_ptr<const string> spExampleString, const char* pStr) const
    {
        return Iterator(spExampleString, pStr);
    }

}