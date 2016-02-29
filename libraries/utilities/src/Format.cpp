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
    Format::Result Format::Match(const char*& content, const char*& format)
    {
        Parser::Trim(content);
        Parser::Trim(format);

        while (*format != '\0' && *format != '%')
        {
            if (std::isspace(*format) && std::isspace(*content))
            {
                Parser::Trim(content);
                Parser::Trim(format);
            }
            else if (*format == *content)
            {
                ++format;
                ++content;
            }
            else if (*content == '\0')
            {
                return Result::earlyEndOfContent;
            }
            else
            {
                return Result::mismatch;
            }
        }

        // the end of a format string must match either the end of content or whitespace in content
        if (*format == '\0' && *content != '\0' && std::isspace(*content) == false)
        {
            return Result::formatEndDoesNotMatchSpace;
        }

        return Result::success;
    }
}