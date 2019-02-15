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
    /// <summary> Checks for a substring in a string (using case-sensitive comparison) </summary>
    ///
    /// <param name="s"> The string to search </param>
    /// <param name="substring"> The substring to search for </param>
    /// <returns> `true` if the substring is contained in the string (according to `std::string::find`) </returns>
    bool Contains(const std::string& s, const std::string& substring);

    /// <summary> Returns copy of std::string with all lowercase characters </summary>
    ///
    /// <param name="s"> The string to convert to lowercase </param>
    /// <returns> Copy of std::string with all lowercase characters </returns>
    std::string ToLowercase(const std::string& s);

    /// <summary> Returns copy of std::string with all uppercase characters </summary>
    ///
    /// <param name="s"> The string to convert to uppercase </param>
    /// <returns> Copy of std::string with all uppercase characters </returns>
    std::string ToUppercase(const std::string& s);

    /// <summary> A printf-style format string function </summary>
    ///
    /// <param name="format"> A printf-style format string </param>
    /// <param name="arg1"> First in a variable list of arguments. </param>
    /// <param name="args"> The rest of the variable list of arguments. </param>
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
    /// <param name="s"> The string to search for substring matches </param>
    /// <param name="from"> The substring to look for </param>
    /// <param name="to"> The string to replace all 'from' instances with </param>
    /// <returns> The modified string with all 'from' instances replaced with the 'to' string </returns>
    void ReplaceAll(std::string& s, const std::string& from, const std::string& to);

    /// <summary> Split a given string at each instance of the given separator character. </summary>
    ///
    /// <param name="s"> The string to search for substring matches </param>
    /// <param name="separator"> The separator character to search for </param>
    /// <returns> A vector of strings found in the given string (not including the separator characters) </returns>
    std::vector<std::string> Split(const std::string& s, char separator);

    /// <summary> Join a vector of strings with a given separator character. </summary>
    ///
    /// <param name="strings"> The vector of strings to join. </param>
    /// <param name="separator"> The separator character to insert between the strings. </param>
    /// <returns> A single string with the given strings separated by the given separator character. </returns>
    std::string Join(const std::vector<std::string>& strings, const std::string& separator);

    /// <summary> Modify a given string so it becomes a valid C language identifier. </summary>
    ///
    /// <param name="s"> The string to valiate. </param>
    /// <returns> A valid C language identifier or empty string if there are no valid chars. </returns>
    std::string MakeValidIdentifier(const std::string& s);

    /// <summary> A templatized helper method that converts a string to given typed value. </summary>
    ///
    /// <typeparam name="ValueType"> The type of the object </typeparam>
    /// <param name="s"> The string to convert. </param>
    /// <returns> A typed value. </returns>
    template <typename T>
    T FromString(const std::string& s);

    template <>
    bool FromString(const std::string& s);

    template <>
    int FromString(const std::string& s);

    template <>
    long FromString(const std::string& s);

    template <>
    unsigned long FromString(const std::string& s);

    template <>
    float FromString(const std::string& s);

    template <>
    double FromString(const std::string& s);

    template <>
    std::string FromString(const std::string& s);
} // namespace utilities
} // namespace ell
