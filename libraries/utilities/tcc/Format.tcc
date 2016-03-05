////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StringFormat.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Parser.h"

// stl
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <type_traits>

namespace utilities
{
    namespace Format
    {
        Match::Match(const char* pStr) : _pStr(pStr)
        {}

        Match::Match(const std::string & str) : _pStr(str.c_str())
        {}

        Match::operator const char*()
        {
            return _pStr;
        }

        template<typename ArgType, typename ... ArgTypes>
        void Printf(std::ostream& os, const char* format, const ArgType& arg, const ArgTypes& ...args)
        {
            if(*format == '\0')
            {
                return;
            }

            while(*format != '%' && *format != '\0')
            {
                if(*format != '^')
                {
                    os << *format;
                }
                ++format;
            }

            if(*format == '%')
            {
                ++format;
                os << arg;
            }

            Printf(os, format, args...);
        }

        template<typename ... ArgTypes>
        std::string Printf(const char* format, const ArgTypes& ...args)
        {
            std::stringstream ss;
            Printf(ss, format, args...);
            return ss.str();
        }

        template<typename ... ArgTypes>
        Result MatchScanf(const char*& content, const char* format, Match match, ArgTypes& ...args)
        {
            auto matchResult = MatchToVariableSubstitution(content, format);

            if(matchResult != Result::success)
            {
                return matchResult;
            }
            if(*format == '\0')
            {
                return Result::success;
            }

            // *format = '%'
            ++format;

            const char* cStr = match;
            matchResult = MatchToVariableSubstitution(content, cStr);
            if(matchResult != Result::success)
            {
                return matchResult;
            }
            if(*cStr != '\0')
            {
                return Result::unexpectedPercentSymbol;
            }

            return MatchScanf(content, format, args...);
        }

        template<typename ArgType, typename ... ArgTypes>
        Result MatchScanf(const char*& content, const char* format, ArgType& arg, ArgTypes& ...args)
        {
            auto matchResult = MatchToVariableSubstitution(content, format);

            if(matchResult != Result::success)
            {
                return matchResult;
            }
            if(*format == '\0')
            {
                return Result::success;
            }

            // *format = '%'
            ++format;

            auto parserResult = Parser::Parse<std::remove_reference_t<ArgType>>(content, arg);
            if(parserResult != Parser::Result::success)
            {
                return Result::parserError;
            }

            return MatchScanf(content, format, args...);
        }

        template<typename ... ArgTypes>
        void MatchScanfThrowsExceptions(const char*& content, const char* format, ArgTypes& ...args)
        {
            auto result = MatchScanf(content, format, args...);

            if(result == Result::success)
            {
                return;
            }

            std::string contentSnippet(content, 30);
            std::string formatSnippet(format,30);
            auto snippets = "\"" + contentSnippet + "\" and \"" + formatSnippet + "\"";

            switch(result)
            {
            case Result::earlyEndOfContent:
                throw std::runtime_error("Error scanning text: content ended before format near: \"" + formatSnippet + "\"");

            case Result::mismatch:
                throw std::runtime_error("Error scanning text: mismatch between content and format near: " + snippets);

            case Result::parserError:
                throw std::runtime_error("Error scanning text: parser error near: " + snippets);

            case Result::missingArgument:
                throw std::runtime_error("Error scanning text: missing argument near: " + snippets);

            case Result::unexpectedPercentSymbol:
                throw std::runtime_error("Error scanning text: unexpected symbol '%' in string argument near: " + snippets);
            }
        }
    }
}
