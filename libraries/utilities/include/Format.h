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
        template<typename ArgType, typename ... ArgTypes>
        Result Printf(std::ostream& os, const char* format, ArgType arg, ArgTypes ...args);

        /// <summary> Formats a string, similar to printf (end condition in the template recurrsion). </summary>
        ///
        /// <param name="os"> [in,out] Stream to write to. </param>
        /// <param name="format"> The format. </param>
        Result Printf(std::ostream& os, const char* format);

        /// <summary> Formats a string, similar to printf </summary>
        ///
        /// <typeparam name="ArgTypes"> Type of the argument types. </typeparam>
        /// <param name="format"> The format. </param>
        /// <param name="...args"> The arguments that are inserted in the format. </param>
        ///
        /// <returns> A std::string. </returns>
        template<typename ... ArgTypes>
        std::string Printf(const char* format, ArgTypes ...args);

        template<typename ... ArgTypes>
        Result Match(const char*& content, const char* format, const char* cStr, ArgTypes ...args);

        template<typename ArgType, typename ... ArgTypes>
        Result MatchScanf(const char* content, const char* format, ArgType& arg, ArgTypes& ...args);

        Result MatchScanf(const char* content, const char* format);

        Result Match(const char*& content, const char*& format);


    }
}

#include "../tcc/Format.tcc"
