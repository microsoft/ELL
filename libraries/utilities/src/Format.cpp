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
    Format::Result Format::MatchStrings(const char*& content, const char*& format)
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
                return Format::Result::earlyEndOfContent;
            }
            else
            {
                return Format::Result::mismatch;
            }
        }

        while (std::isspace(*content))
        {
            ++content;
        }

        if (*format == '\0' && *content != '\0')
        {
            return Format::Result::earlyEndOfFormat;
        }

        return Format::Result::success;
    }
}