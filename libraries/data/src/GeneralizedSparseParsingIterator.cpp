////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GeneralizedSparseParsingIterator.cpp (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GeneralizedSparseParsingIterator.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace data
{
    GeneralizedSparseParsingIterator::GeneralizedSparseParsingIterator(TextLine& textLine) : _textLine(textLine)
    {
        _textLine.TrimLeadingWhitespace();
        if (_textLine.IsEndOfContent())
        {
            _isValid = false;
        }
        else
        {
            ReadEntry(0);
            _textLine.TrimLeadingWhitespace();
        }
    }

    void GeneralizedSparseParsingIterator::Next()
    {
        if (_textLine.IsEndOfContent())
        {
            _isValid = false;
        }
        else
        {
            ReadEntry(_currentIndexValue.index + 1);
            _textLine.TrimLeadingWhitespace();
        }
    }

    void GeneralizedSparseParsingIterator::ReadEntry(size_t nextIndex)
    {
        // check for prefix '+'
        bool firstCharacterIsPlus = false;
        if (_textLine.Peek() == '+')
        {
            firstCharacterIsPlus = true;
            _textLine.AdvancePosition();
        }

        // set index
        _currentIndexValue.index = nextIndex;

        // read integer
        size_t integerPart;
        auto stepSize = _textLine.TryParse(integerPart);

        // case 1: can't parse as integer, re-parse as double (e.g ".3")
        if (stepSize == 0)
        {
            _textLine.ParseAdvance(_currentIndexValue.value);
            return;
        }

        char nextChar = _textLine.Peek(stepSize);

        // case 2: the parsed integer is an index, followed by ':<value>'
        if (nextChar == ':')
        {
            _textLine.AdvancePosition(stepSize+1);

            // relative index or absolute index
            if (firstCharacterIsPlus)
            {
                if (integerPart == 0)
                {
                    throw utilities::DataFormatException(utilities::DataFormatErrors::illegalValue, "relative index cannot equal zero");
                }
                _currentIndexValue.index = nextIndex + integerPart - 1;
            }
            else
            {
                if (integerPart < nextIndex)
                {
                    throw utilities::DataFormatException(utilities::DataFormatErrors::illegalValue, "absolute index cannot be smaller than previous index");
                }
                _currentIndexValue.index = integerPart;
            }

            _textLine.ParseAdvance(_currentIndexValue.value);
        }

        // case 3: the parsed integer is the value - cast it to double
        else if (std::isspace(nextChar) != 0 || nextChar == '\0')
        {
            _currentIndexValue.value = static_cast<double>(integerPart);
            _textLine.AdvancePosition(stepSize);
            return;
        }

        // case 4: something else happened, so re-parse the value as a double (e.g. "1.3")
        else
        {
            _textLine.ParseAdvance(_currentIndexValue.value);
        }
    }
}
}
