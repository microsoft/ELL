////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StringFormat.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <ostream>
#include <string>

namespace ell
{
namespace utilities
{
    const char substitutionSymbol = '%';
    const char whitespaceSymbol = '^';

    /// <summary> Prints formatted text to an ostream, similar to the C function printf. </summary>
    ///
    /// <typeparam name="ArgType"> Type of the first argument. </typeparam>
    /// <typeparam name="ArgTypes"> Types of the remaining arguments. </typeparam>
    /// <param name="os"> [in,out] Stream to write to. </param>
    /// <param name="format"> The format string. </param>
    /// <param name="arg"> The first argument to be inserted in the format. </param>
    /// <param name="...args"> The remaining arguments that are inserted in the format. </param>
    template <typename ArgType, typename... ArgTypes>
    void PrintFormat(std::ostream& os, const char* format, const ArgType& arg, const ArgTypes&... args);

    /// <summary>
    /// Prints formatted text to an ostream, similar to the C function printf. (end condition in the
    /// template recurrsion).
    /// </summary>
    ///
    /// <param name="os"> [in,out] Stream to write to. </param>
    /// <param name="format"> The format string. </param>
    void PrintFormat(std::ostream& os, const char* format);

    /// <summary> Formats a string, similar to the C function printf </summary>
    ///
    /// <typeparam name="ArgTypes"> Type of the argument types. </typeparam>
    /// <param name="format"> The format. </param>
    /// <param name="...args"> The arguments that are inserted in the format. </param>
    ///
    /// <returns> A std::string. </returns>
    template <typename... ArgTypes>
    std::string PrintFormat(const char* format, const ArgTypes&... args);

    /// <summary> A class used to indicate which arguments to MatchFormat are matched, rather than extracted from the content. </summary>
    class Match
    {
    public:
        /// <summary> Constructs an instance of Match. </summary>
        ///
        /// <param name="pStr"> The string to match. </param>
        inline explicit Match(const char* pStr);

        /// <summary> Constructs an instance of Match. </summary>
        ///
        /// <param name="str"> The string to match. </param>
        inline explicit Match(const std::string& str);

        /// <summary> Cast operator to const char*. </summary>
        ///
        /// <returns> A const char*. </returns>
        inline operator const char*();

    private:
        const char* _pStr;
    };

    /// <summary>
    /// Results of Match and Scanf. success = match and parse of all arguments successful,
    /// earlyEndOfContent = content ended before format, mismatch = character in content does not
    /// match the one in format, parserError = parse failed, missingArgument = format has more %
    /// symbols than arguments provided, unexpectedPercentSymbol = when an argument of Match contains
    /// the % symbol.
    /// </summary>
    enum class MatchResult
    {
        success,
        earlyEndOfContent,
        mismatch,
        parserError,
        missingArgument,
        unexpectedPercentSymbol
    };

    /// <summary> Matches content to a format, allowing arbitrary whitespace, until a % character is
    /// encountered. </summary>
    ///
    /// <param name="content"> The content. </param>
    /// <param name="format"> The format to use. </param>
    ///
    /// <returns> Either MatchResult::success or an error code. </returns>
    MatchResult MatchToSubstitutionSymbol(const char*& content, const char*& format);

    /// <summary> Match content to a format, while parsing values in the position where the format
    /// contains the character %. </summary>
    ///
    /// <typeparam name="ArgTypes"> Type of the remaining arguments to parse. </typeparam>
    /// <param name="content"> The content string. </param>
    /// <param name="format"> The format string, which includes the character % in the posiitons that
    ///  should be parsed. </param>
    /// <param name="match"> A reference to the first argument that must be parsed from the content. </param>
    /// <param name="...args"> [in,out] A reference to the remaining arguments that must be parsed
    ///  from the content. </param>
    ///
    /// <returns> Either MatchResult::success or an error code. </returns>
    template <typename... ArgTypes>
    MatchResult MatchFormat(const char*& content, const char* format, Match match, ArgTypes&... args);

    /// <summary> Match content to a format, while parsing values in the position where the format
    /// contains the character %. </summary>
    ///
    /// <typeparam name="ArgType"> Type of the first argument to parse. </typeparam>
    /// <typeparam name="ArgTypes"> Type of the remaining arguments to parse. </typeparam>
    /// <param name="content"> The content string. </param>
    /// <param name="format"> The format string, which includes the character % in the posiitons that
    ///  should be parsed. </param>
    /// <param name="arg"> [in,out] A reference to the first argument that must be parsed from the
    ///  content. </param>
    /// <param name="...args"> [in,out] A reference to the remaining arguments that must be parsed
    ///  from the content. </param>
    ///
    /// <returns> Either MatchResult::success or an error code. </returns>
    template <typename ArgType, typename... ArgTypes>
    MatchResult MatchFormat(const char*& content, const char* format, ArgType& arg, ArgTypes&... args);

    /// <summary> Match content to a format, while parsing values in the position where the format
    /// contains the character % (termination). </summary>
    ///
    /// <param name="content"> The content string. </param>
    /// <param name="format"> The format string, which includes the character % in the positions that
    ///  should be parsed. </param>
    ///
    /// <returns> Either MatchResult::success or an error code. </returns>
    MatchResult MatchFormat(const char*& content, const char* format);

    /// <summary> Match content to a format, while parsing values in the position where the format
    /// contains the character %. This version of the function throws an exception when it
    /// encounters a mismatch or a parsing error. </summary>
    ///
    /// <typeparam name="ArgType"> Type of the first argument to parse. </typeparam>
    /// <typeparam name="ArgTypes"> Type of the remaining arguments to parse. </typeparam>
    /// <param name="content"> The content string. </param>
    /// <param name="format"> The format string, which includes the character % in the posiitons that
    ///  should be parsed. </param>
    /// <param name="arg"> [in,out] A reference to the first argument that must be parsed from the
    ///  content. </param>
    /// <param name="...args"> [in,out] A reference to the remaining arguments that must be parsed
    ///  from the content. </param>
    ///
    /// <returns> Either MatchResult::success or an error code. </returns>
    template <typename... ArgTypes>
    void MatchFormatThrowsExceptions(const char*& content, const char* format, ArgTypes&&... args);
}
}

#include "../tcc/Format.tcc"
