////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StringUtil.h (utilities)
//  Authors:  Chris Lovett, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> Returns copy of std::string with all lowercase characters </summary>
    ///
    /// <returns> Copy of std::string with all lowercase characters </returns>
    std::string ToLowercase(const std::string& s);

    /// <summary> A printf-style format string function </summary>
    ///
    /// <returns> A std::string object containing the formatted string </returns>
    template <typename... Args>
    std::string FormatString(const char* format, Args&&... args)
    {
        size_t size = std::snprintf(nullptr, 0, format, std::forward<Args>(args)...) + 1;
        std::vector<char> buf(size + 1);
        std::snprintf(buf.data(), buf.size(), format, std::forward<Args>(args)...);
        return std::string(buf.data());
    }

    /// <summary> Replace all instances of the 'from' substring in 'str' with the value of the 'to' string. </summary>
    void ReplaceAll(std::string& str, const std::string& from, const std::string& to);
}
}