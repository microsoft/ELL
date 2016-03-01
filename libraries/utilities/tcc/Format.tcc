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
#include <type_traits>

namespace utilities
{
    namespace Format
    {
        Match::Match(const char* pStr) : _pStr(pStr)
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

            const char* ptr = format;
            while(*ptr != '%' && *ptr != '^' && *ptr != '\0')
            {
                ++ptr;
            }

            os.write(format, (ptr - format));

            if(*ptr == '%')
            {
                ++ptr;
                os << arg;
            }
            else if(*ptr == '^')
            {
                ++ptr;
            }

            return Printf(os, ptr, args...);
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
            auto matchResult = FindPercent(content, format);

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
            matchResult = FindPercent(content, cStr);
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
            auto matchResult = FindPercent(content, format);

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
    }
}
