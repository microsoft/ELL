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
        enum class Result {success, earlyEndOfFormat, earlyEndOfContent, mismatch, parserError};

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
        /// <typeparam name="ArgTypes"> Type of the argument types. </typeparam>
        /// <param name="os"> [in,out] Stream to write to. </param>
        /// <param name="format"> The format. </param>
        /// <param name="...args"> The arguments that are inserted in the format. </param>
        template<typename ... ArgTypes>
        Result Printf(std::ostream& os, const char* format, ArgTypes ...args);

        /// <summary> Formats a string, similar to printf </summary>
        ///
        /// <typeparam name="ArgTypes"> Type of the argument types. </typeparam>
        /// <param name="format"> The format. </param>
        /// <param name="...args"> The arguments that are inserted in the format. </param>
        ///
        /// <returns> A std::string. </returns>
        template<typename ... ArgTypes>
        std::string Printf(const char* format, ArgTypes ...args);

        Result MatchStrings(const char*& content, const char*& format);

        template<typename ArgType, typename ... ArgTypes>
        Result MatchScanf(const char* content, const char* format, ArgType& arg, ArgTypes& ...args);

        template<typename ... ArgTypes>
        Result MatchScanf(const char* content, const char* format, ArgTypes& ...args);

    }
}

#include "../tcc/Format.tcc"
