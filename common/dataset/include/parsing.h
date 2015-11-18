// parsing.h

#pragma once

#include "types.h"
using linear::uint;

namespace dataset
{
    /// The results of the parse command
    /// success         Parsing succeeded
    /// badFormat       The string was not formatted correctly
    /// endOfString     The pointer pStr points \0 or to whitespace followed by \0
    /// outOfRange      The number was in correct format but its value exceeds the range of the specified type
    /// beginComment    The pStr string starts with "//" or "#", perhaps with preceding before
    enum class ParseResults { success, badFormat, endOfString, outOfRange, beginComment};

    /// Parses numbers in a c-string. 
    /// For example: 
    ///     std::string stlString = "3.14    7   "
    ///     const char* pStr = stlString.c_str(); 
    ///     double x;
    ///     uint y;
    ///     uint z;
    ///     auto result = parse(pStr, /* out */ x);
    ///     assert(result == ParseResults::success && x == 3.14);
    ///     result = parse(pStr, /* out */ y);
    ///     assert(result == ParseResults::success && y == 7);
    ///     result = parse(pStr, /* out */ y);
    ///     assert(result == ParseResults::endOfString);
    template<typename ValueType>
    ParseResults parse(const char*& pStr, /* out */ double& value);

    /// Advances pStr until it points to a non-whitespace character
    ///
    inline void trim(const char*& pStr);
}

#include "../tcc/parsing.tcc"
