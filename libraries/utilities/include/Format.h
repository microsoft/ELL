////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StringFormat.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// stl
#include <iostream>

namespace utilities
{
    namespace Format
    {
        /// <summary>
        /// Results of Match and Scanf. success = match and parse of all arguments successful,
        /// formatEndDoesNotMatchSpace = end of format must match end of content or whitespace,
        /// earlyEndOfContent = content ended before format, mismatch = character in content does not
        /// match the one in format, parserError = parse failed, missingArgument = format has more %
        /// symbols than arguments provided, unexpectedPercentSymbol = when an argument of Match contains
        /// the % symbol.
        /// </summary>
        enum class Result {success, formatEndDoesNotMatchSpace, earlyEndOfContent, mismatch, parserError, missingArgument, unexpectedPercentSymbol};

        /// <summary> Formats a string, similar to printf. </summary>
        ///
        /// <typeparam name="ArgType"> Type of the first argument. </typeparam>
        /// <typeparam name="ArgTypes"> Types of the remaining arguments. </typeparam>
        /// <param name="os"> [in,out] Stream to write to. </param>
        /// <param name="format"> The format. </param>
        /// <param name="arg"> The first argument. </param>
        /// <param name="...args"> The remaining arguments that are inserted in the format. </param>
        ///
        /// <returns> Either Result::success or an error code. </returns>
        template<typename ArgType, typename ... ArgTypes>
        Result Printf(std::ostream& os, const char* format, const ArgType& arg, const ArgTypes& ...args);

        /// <summary>
        /// Formats a string, similar to printf (end condition in the template recurrsion).
        /// </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        /// <param name="format"> The format. </param>
        ///
        /// <returns> Either Result::success or an error code. </returns>
        Result Printf(std::ostream& os, const char* format);

        /// <summary> Formats a string, similar to printf </summary>
        ///
        /// <typeparam name="ArgTypes"> Type of the argument types. </typeparam>
        /// <param name="format"> The format. </param>
        /// <param name="...args"> The arguments that are inserted in the format. </param>
        ///
        /// <returns> A std::string. </returns>
        template<typename ... ArgTypes>
        std::string Printf(const char* format, const ArgTypes& ...args);

        /// <summary>
        /// Tests if the content matches a format string. Each time the format has a % character, one of
        /// the argument strings is inserted.
        /// </summary>
        ///
        /// <typeparam name="ArgTypes"> Type of the arguments - this is always const char*. </typeparam>
        /// <param name="content"> The content string. </param>
        /// <param name="format"> The format to use. </param>
        /// <param name="cStr"> The first string to drop in instead of %. </param>
        /// <param name="...args"> The remaining strings to drop in instead of %. </param>
        ///
        /// <returns> Either Result::success or an error code. </returns>
        template<typename ... ArgTypes>
        Result Match(const char*& content, const char* format, const char* cStr, ArgTypes ...args);

        /// <summary> Matches content to a format, allowing arbitrary whitespace, until a % character is encountered </summary>
        ///
        /// <param name="content"> The content. </param>
        /// <param name="format"> The format to use. </param>
        ///
        /// <returns> Either Result::success or an error code. </returns>
        Result Match(const char*& content, const char*& format);

        /// <summary> Match content to a format, while parsing values in the position where the format contains the character %. </summary>
        ///
        /// <typeparam name="ArgType"> Type of the first argument to parse. </typeparam>
        /// <typeparam name="ArgTypes"> Type of the remaining arguments to parse. </typeparam>
        /// <param name="content"> The content string. </param>
        /// <param name="format"> The format string, which includes the character % in the posiitons that should be parsed. </param>
        /// <param name="arg"> [in,out] A reference to the first argument that must be parsed from the content. </param>
        /// <param name="...args"> [in,out] A reference to the remaining arguments that must be parsed from the content. </param>
        ///
        /// <returns> Either Result::success or an error code. </returns>
        template<typename ArgType, typename ... ArgTypes>
        Result MatchScanf(const char*& content, const char* format, ArgType& arg, ArgTypes& ...args);

        /// <summary> Match content to a format, while parsing values in the position where the format contains the character % (termination). </summary>
        ///
        /// <param name="content"> The content string. </param>
        /// <param name="format"> The format string, which includes the character % in the posiitons that should be parsed. </param>
        ///
        /// <returns> Either Result::success or an error code. </returns>
        Result MatchScanf(const char*& content, const char* format);
    }
}

#include "../tcc/Format.tcc"
