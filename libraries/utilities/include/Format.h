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

        /// <summary> A class used to indicate which arguments to MatchScanf are matched, rather than extracted from the content. </summary>
        class Match 
        {
        public:

            /// <summary> Constructs an instance of Match. </summary>
            ///
            /// <param name="pStr"> The string to match. </param>
            inline explicit Match(const char* pStr);

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
        enum class Result {success, earlyEndOfContent, mismatch, parserError, missingArgument, unexpectedPercentSymbol};

        /// <summary> Formats a string, similar to printf. </summary>
        ///
        /// <typeparam name="ArgType"> Type of the first argument. </typeparam>
        /// <typeparam name="ArgTypes"> Types of the remaining arguments. </typeparam>
        /// <param name="os"> [in,out] Stream to write to. </param>
        /// <param name="format"> The format. </param>
        /// <param name="arg"> The first argument. </param>
        /// <param name="...args"> The remaining arguments that are inserted in the format. </param>
        template<typename ArgType, typename ... ArgTypes>
        void Printf(std::ostream& os, const char* format, const ArgType& arg, const ArgTypes& ...args);

        /// <summary>
        /// Formats a string, similar to printf (end condition in the template recurrsion).
        /// </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        /// <param name="format"> The format. </param>
        void Printf(std::ostream& os, const char* format);

        /// <summary> Formats a string, similar to printf </summary>
        ///
        /// <typeparam name="ArgTypes"> Type of the argument types. </typeparam>
        /// <param name="format"> The format. </param>
        /// <param name="...args"> The arguments that are inserted in the format. </param>
        ///
        /// <returns> A std::string. </returns>
        template<typename ... ArgTypes>
        std::string Printf(const char* format, const ArgTypes& ...args);

        /// <summary> Matches content to a format, allowing arbitrary whitespace, until a % character is
        /// encountered. </summary>
        ///
        /// <param name="content"> The content. </param>
        /// <param name="format"> The format to use. </param>
        ///
        /// <returns> Either Result::success or an error code. </returns>
        Result FindPercent(const char*& content, const char*& format);

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
        /// <returns> Either Result::success or an error code. </returns>
        ///
        /// ### <typeparam name="ArgType"> Type of the first argument to parse. </typeparam>
        template<typename ... ArgTypes>
        Result MatchScanf(const char*& content, const char* format, Match match, ArgTypes& ...args);

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
        /// <returns> Either Result::success or an error code. </returns>
        template<typename ArgType, typename ... ArgTypes>
        Result MatchScanf(const char*& content, const char* format, ArgType& arg, ArgTypes& ...args);

        /// <summary> Match content to a format, while parsing values in the position where the format
        /// contains the character % (termination). </summary>
        ///
        /// <param name="content"> The content string. </param>
        /// <param name="format"> The format string, which includes the character % in the positions that
        ///  should be parsed. </param>
        ///
        /// <returns> Either Result::success or an error code. </returns>
        Result MatchScanf(const char*& content, const char* format);

        /// <summary> Match content to a format, while parsing values in the position where the format
        /// contains the character %. This version of the function throws a runtime_error when it
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
        /// <returns> Either Result::success or an error code. </returns>
        template<typename ... ArgTypes>
        void MatchScanfThrowsExceptions(const char*& content, const char* format, ArgTypes& ...args);
    }
}

#include "../tcc/Format.tcc"
