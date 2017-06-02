////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Format.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Format.h"

namespace ell
{
namespace utilities
{
    void PrintFormat(std::ostream& os, const char* format)
    {
        if (*format == '\0')
        {
            return;
        }

        while (*format != '\0')
        {
            if (*format != whitespaceSymbol)
            {
                os << *format;
            }
            ++format;
        }
    }

    MatchResult MatchToSubstitutionSymbol(const char*& content, const char*& format)
    {
        while (*format != '\0' && *format != substitutionSymbol)
        {
            if (std::isspace(*format) && std::isspace(*content))
            {
                TrimLeadingWhitespace(content);
                TrimLeadingWhitespace(format);
            }
            else if (*format == whitespaceSymbol)
            {
                TrimLeadingWhitespace(content);
                ++format;
            }
            else if (*format == *content)
            {
                ++format;
                ++content;
            }
            else if (*content == '\0')
            {
                return MatchResult::earlyEndOfContent;
            }
            else
            {
                return MatchResult::mismatch;
            }
        }

        return MatchResult::success;
    }

    MatchResult MatchFormat(const char*& content, const char* format)
    {
        auto matchResult = MatchToSubstitutionSymbol(content, format);
        if (matchResult != MatchResult::success)
        {
            return matchResult;
        }

        if (*format != '\0')
        {
            return MatchResult::missingArgument;
        }

        return MatchResult::success;
    }
}
}
