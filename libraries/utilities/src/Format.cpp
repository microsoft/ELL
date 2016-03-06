////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Format.cpp (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Format.h"

namespace utilities
{
    void PrintFormat(std::ostream& os, const char* format)
    {
        if(*format == '\0')
        {
            return;
        }

        while(*format != '\0')
        {
            if(*format != whitespaceSymbol)
            {
                os << *format;
            }
            ++format;
        }
    }

    MatchResult MatchToSubstitutionSymbol(const char*& content, const char*& format)
    {
        while(*format != '\0' && *format != substitutionSymbol)
        {
            if(std::isspace(*format) && std::isspace(*content))
            {
                Trim(content);
                Trim(format);
            }
            else if(*format == whitespaceSymbol)
            {
                Trim(content);
                ++format;
            }
            else if(*format == *content)
            {
                ++format;
                ++content;
            }
            else if(*content == '\0')
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
        auto MatchResult = MatchToSubstitutionSymbol(content, format);
        if(MatchResult != MatchResult::success)
        {
            return MatchResult;
        }

        if(*format != '\0')
        {
            return MatchResult::missingArgument;
        }

        return MatchResult::success;
    }
}

