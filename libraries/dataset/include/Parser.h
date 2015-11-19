// Parser.h

#pragma once

#include "types.h"


namespace dataset
{
    class Parser
    {
    public:
        /// The results of the parse command
        /// success         Parsing succeeded
        /// badFormat       The string was not formatted correctly
        /// endOfString     The pointer pStr points \0 or to whitespace followed by \0
        /// outOfRange      The number was in correct format but its value exceeds the range of the specified type
        /// beginComment    The pStr string starts with "//" or "#", perhaps with preceding before
        enum class Result { success, badFormat, endOfString, outOfRange, beginComment };

        /// Parses numbers in a c-string. 
        /// For example: 
        ///     std::string stlString = "3.14    7   "
        ///     const char* pStr = stlString.c_str(); 
        ///     double x;
        ///     uint64 y;
        ///     uint64 z;
        ///     auto result = parse(pStr, /* out */ x);
        ///     assert(result == Parser::Result::success && x == 3.14);
        ///     result = parse(pStr, /* out */ y);
        ///     assert(result == Parser::Result::success && y == 7);
        ///     result = parse(pStr, /* out */ y);
        ///     assert(result == Parser::Result::endOfString);
        template<typename ValueType>
        static Result Parse(const char*& pStr, /* out */ ValueType& value);

        /// Advances pStr until it points to a non-whitespace character
        ///
        static inline void Trim(const char*& pStr);
    };
}

#include "../tcc/Parser.tcc"
