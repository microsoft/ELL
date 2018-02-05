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
    /// <param name=s> The string to convert to lowercase </param>
    /// <returns> Copy of std::string with all lowercase characters </returns>
    std::string ToLowercase(const std::string& s);

    /// <summary> A printf-style format string function </summary>
    ///
    /// <param name=format> A printf-style format string </param>
    /// <param name=arg1> First in a variable list of arguments. </param>
    /// <returns> A std::string object containing the formatted string </returns>
    template <typename Arg1, typename... Args>
    std::string FormatString(const char* format, Arg1&& arg1, Args&&... args)
    {
        size_t size = std::snprintf(nullptr, 0, format, std::forward<Arg1>(arg1), std::forward<Args>(args)...) + 1;
        std::vector<char> buf(size);
        std::snprintf(buf.data(), buf.size(), format, std::forward<Arg1>(arg1), std::forward<Args>(args)...);
        return std::string(buf.data());
    }

    /// <summary> Replace all instances of the 'from' substring in 'str' with the value of the 'to' string. </summary>
    ///
    /// <param name=str> The string to search for substring matches </param>
    /// <param name=from> The substring to look for </param>
    /// <param name=to> The string to replace all 'from' instances with </param>
    /// <returns> The modified string with all 'from' instances replaced with the 'to' string </returns>
    void ReplaceAll(std::string& str, const std::string& from, const std::string& to);

    /// <summary> Split a given string at each instance of the given separator character. </summary>
    ///
    /// <param name=s> The string to search for substring matches </param>
    /// <param name=separator> The separator character to search for </param>
    /// <returns> A vector of strings found in the given string (not including the separator characters) </returns>
    std::vector<std::string> Split(const std::string& s, char separator);
}
}
