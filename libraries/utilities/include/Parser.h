////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Parser.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace utilities
{
    namespace Parser
    {
        /// <summary> The results of the parse command: success = Parsing succeeded; badFormat = The string
        /// was not formatted correctly; endOfString = The pointer pStr points \0 or to whitespace
        /// followed by \0; outOfRange = The number was in correct format but its value exceeds the range
        /// of the specified type; beginComment = The pStr string starts with "//" or "#", perhaps with
        /// preceding before. </summary>
        enum class Result { success, badFormat, endOfString, outOfRange, beginComment };

        /// <summary> Parses numbers in a c-string and advances the string pointer. </summary>
        ///
        /// <typeparam name="ValueType"> Type of number to output. </typeparam>
        /// <param name="pStr"> The string pointer. </param>
        /// <param name="value"> [in,out] The value. </param>
        ///
        /// <returns> A Result. </returns>
        template<typename ValueType>
        Result Parse(const char*& pStr, /* out */ ValueType& value);

        /// <summary> Advances pStr until it points to a non-whitespace character. </summary>
        ///
        /// <param name="pStr"> The string pointer. </param>
        inline void Trim(const char*& pStr);
    };
}

#include "../tcc/Parser.tcc"
