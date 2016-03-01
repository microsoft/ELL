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
    namespace Format
    {
        void Printf(std::ostream& os, const char* format)
        {
            if(*format == '\0')
            {
                return;
            }

            const char* ptr = format;
            while(*ptr != '^' && *ptr != '\0')
            {
                ++ptr;
            }

            os.write(format, (ptr - format));

            if(*ptr == '^')
            {
                ++ptr;
                return Printf(os, ptr);
            }
        }

        Result FindPercent(const char*& content, const char*& format)
        {
            while(*format != '\0' && *format != '%')
            {
                if(std::isspace(*format) && std::isspace(*content))
                {
                    Parser::Trim(content);
                    Parser::Trim(format);
                }
                else if(*format == '^')
                {
                    Parser::Trim(content);
                    ++format;
                }
                else if(*format == *content)
                {
                    ++format;
                    ++content;
                }
                else if(*content == '\0')
                {
                    return Result::earlyEndOfContent;
                }
                else
                {
                    return Result::mismatch;
                }
            }

            return Result::success;
        }

        Result MatchScanf(const char*& content, const char* format)
        {
            auto result = FindPercent(content, format);
            if(result != Result::success)
            {
                return result;
            }

            if(*format != '\0')
            {
                return Result::missingArgument;
            }

            return Result::success;
        }
    }
}