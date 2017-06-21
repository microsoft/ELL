////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CStringParser.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace utilities
{
    /// <summary> The results of the parse command: 
    ///     success = Parsing succeeded;
    ///     badFormat = The string was not formatted correctly;
    ///     endOfString = The pointer pStr points \0 or to whitespace followed by \0;
    ///     outOfRange = The number was in correct format but its value exceeds the range of the specified type;
    ///     beginComment = The pStr string starts with "//" or "#", perhaps with preceding before.
    /// </summary>
    enum class ParseResult
    {
        success,
        badFormat,
        endOfString,
        outOfRange,
        beginComment
    };

    /// <summary> Parses numbers in a c-string and advances the string pointer. </summary>
    ///
    /// <typeparam name="ValueType"> Type of number to output. </typeparam>
    /// <param name="pStr"> The string pointer. </param>
    /// <param name="value"> [in,out] The value. </param>
    ///
    /// <returns> A Result. </returns>
    template <typename ValueType>
    ParseResult Parse(const char*& pStr, ValueType& value);

    /// <summary> Advances pStr until it points to a non-whitespace character. </summary>
    ///
    /// <param name="pStr"> The string pointer. </param>
    void TrimLeadingWhitespace(const char*& pStr);

    /// <summary> Query if a character is the end of string character. </summary>
    ///
    /// <param name="c"> The character. </param>
    ///
    /// <returns> True if end of string, false if not. </returns>
    bool IsEndOfString(char c);

    /// <summary> Query if a character is whitespace. </summary>
    ///
    /// <param name="c"> The character. </param>
    ///
    /// <returns> True if whitespace, false if not. </returns>
    bool IsWhitespace(char c);

    /// <summary> Query if a character is a digit. </summary>
    ///
    /// <param name="c"> The character. </param>
    ///
    /// <returns> True if digit, false if not. </returns>
    bool IsDigit(char c);
}
}

#include "../tcc/CStringParser.tcc"
