////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CStringParser.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <string>

namespace ell
{
namespace utilities
{
    // wrapper for strtof
    inline ParseResult cParse(const char* pStr, char*& pEnd, float& value)
    {
        if (IsWhitespace(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        value = strtof(pStr, &pEnd);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }

        errno = tmp;
        return ParseResult::success;
    }

    // wrapper for std::strtod
    inline ParseResult cParse(const char* pStr, char*& pEnd, double& value)
    {
        if (IsWhitespace(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        value = std::strtod(pStr, &pEnd);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }

        errno = tmp;
        return ParseResult::success;
    }

    // wrapper for strtoul
    inline ParseResult cParse(const char* pStr, char*& pEnd, unsigned int& value)
    {
        if (!IsDigit(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        auto x = strtoul(pStr, &pEnd, 0);
        if (x != static_cast<unsigned int>(x))
        {
            return ParseResult::outOfRange;
        }

        value = static_cast<unsigned int>(x);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }

        errno = tmp;
        return ParseResult::success;
    }

    // wrapper for strtoul
    inline ParseResult cParse(const char* pStr, char*& pEnd, uint64_t& value)
    {
        if (!IsDigit(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        auto x = strtoul(pStr, &pEnd, 0);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }
        if (x != static_cast<uint64_t>(x))
        {
            return ParseResult::outOfRange;
        }

        value = static_cast<uint64_t>(x);

        errno = tmp;
        return ParseResult::success;
    }

    // wrapper for strtol
    inline ParseResult cParse(const char* pStr, char*& pEnd, int& value)
    {
        if (IsWhitespace(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        auto x = strtol(pStr, &pEnd, 0);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }
        if (x != static_cast<int>(x))
        {
            return ParseResult::outOfRange;
        }

        value = static_cast<int>(x);

        errno = tmp;
        return ParseResult::success;
    }

    // wrapper for strtol
    inline ParseResult cParse(const char* pStr, char*& pEnd, long& value)
    {
        if (IsWhitespace(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        value = strtol(pStr, &pEnd, 0);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }

        errno = tmp;
        return ParseResult::success;
    }

    // wrapper for strtoul
    inline ParseResult cParse(const char* pStr, char*& pEnd, unsigned short& value)
    {
        if (!IsDigit(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        auto x = strtoul(pStr, &pEnd, 0);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }
        if (x != static_cast<unsigned short>(x))
        {
            return ParseResult::outOfRange;
        }
        value = static_cast<unsigned short>(x);

        errno = tmp;
        return ParseResult::success;
    }

    // wrapper for strtol
    inline ParseResult cParse(const char* pStr, char*& pEnd, short& value)
    {
        if (IsWhitespace(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        long x = strtol(pStr, &pEnd, 0);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }
        if (x != static_cast<short>(x))
        {
            return ParseResult::outOfRange;
        }

        value = static_cast<short>(x);

        errno = tmp;
        return ParseResult::success;
    }

    // parser for std:string, scans until finding the a character other than alphanumeric or '_'
    inline ParseResult cParse(const char* pStr, char*& pEnd, std::string& value)
    {
        const char* iter = pStr;
        while (std::isalnum(*iter) || *iter == '_')
        {
            ++iter;
        }
        value = std::string(pStr, iter);
        pEnd = const_cast<char*>(iter);

        return ParseResult::success;
    }

    // wrapper for strtoul
    template <typename std::enable_if_t<!std::is_same<unsigned long, unsigned int>::value, int> = 0>
    inline ParseResult cParse(const char* pStr, char*& pEnd, unsigned long& value)
    {
        if (!IsDigit(*pStr))
        {
            return ParseResult::badFormat;
        }

        auto tmp = errno;
        errno = 0;

        value = strtoul(pStr, &pEnd, 0);

        if (pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if (errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }

        errno = tmp;
        return ParseResult::success;
    }

    template <typename ValueType>
    ParseResult Parse(const char*& pStr, ValueType& value)
    {
        // check for eof
        if (IsEndOfString(*pStr))
        {
            return ParseResult::endOfString;
        }

        // check for "//" comment indicator
        if (*pStr == '/')
        {
            if (*(pStr + 1) == '/')
            {
                return ParseResult::beginComment;
            }
        }

        // check for "#" comment indicator
        if (*pStr == '#')
        {
            return ParseResult::beginComment;
        }

        char* pEnd = nullptr;
        auto parseResult = cParse(pStr, pEnd, value);
        pStr = pEnd;

        return parseResult;
    }
}
}
