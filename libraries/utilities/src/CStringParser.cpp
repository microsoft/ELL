////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CStringParser.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CStringParser.h"

// stl
#include <cctype>

namespace ell
{
namespace utilities
{
    void TrimLeadingWhitespace(const char*& pStr)
    {
        while (std::isspace(*pStr))
        {
            ++pStr;
        }
    }

    bool IsEndOfString(char c)
    {
        return c == '\0';
    }

    bool IsWhitespace(char c)
    {
        return std::isspace(c) != 0;
    }

    bool IsDigit(char c)
    {
        return std::isdigit(c) != 0;
    }
}
}