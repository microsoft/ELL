////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     Parser.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cstdlib>
#include <stdexcept>
#include <cctype>

namespace utilities
{
    // wrapper for strtof
    inline void cParse(const char* pStr, char*& pEnd, float& value)
    {
        value = strtof(pStr, &pEnd);
    }

    // wrapper for std::strtod
    inline void cParse(const char* pStr, char*& pEnd, double& value)
    {
        value = std::strtod(pStr, &pEnd);
    }

    // wrapper for stroul
    inline void cParse(const char* pStr, char*& pEnd, unsigned int& value)
    {
        unsigned long x = strtoul(pStr, &pEnd, 0);
        if(x != (unsigned int)x)
        {
            errno = ERANGE;
        }
        value = (unsigned int)x;
    }

    // wrapper for strtoul
    inline void cParse(const char* pStr, char*& pEnd, uint64_t& value)
    {
        unsigned long x = strtoul(pStr, &pEnd, 0);
        if(x != (uint64_t)x)
        {
            errno = ERANGE;
        }
        value = (uint64_t)x;
    }

    // wrapper for strtol
    inline void cParse(const char* pStr, char*& pEnd, int& value)
    {
        long x = strtol(pStr, &pEnd, 0);
        if(x != (int)x)
        {
            errno = ERANGE;
        }
        value = (int)x;
    }

    // wrapper for strtoul
    template <typename std::enable_if_t<!std::is_same<unsigned long, unsigned int>::value, int> = 0>
    inline void cParse(const char* pStr, char*& pEnd, unsigned long& value)
    {
        value = strtoul(pStr, &pEnd, 0);
    }

    // wrapper for strtol
    inline void cParse(const char* pStr, char*& pEnd, long& value)
    {
        value = strtol(pStr, &pEnd, 0);
    }

    // wrapper for strtoul
    inline void cParse(const char* pStr, char*& pEnd, unsigned short& value)
    {
        unsigned long x = strtoul(pStr, &pEnd, 0);
        if(x != (unsigned short)x)
        {
            errno = ERANGE;
        }
        value = (unsigned short)x;
    }

    // wrapper for strtol
    inline void cParse(const char* pStr, char*& pEnd, short& value)
    {
        long x = strtol(pStr, &pEnd, 0);
        if(x != (short)x)
        {
            errno = ERANGE;
        }
        value = (short)x;
    }

    // parser for std:string, scans until finding the a character other than alphanumeric or '_'
    inline void cParse(const char* pStr, char*& pEnd, std::string& value)
    {
        const char* iter = pStr;
        while (std::isalnum(*iter) || *iter == '_')
        {
            ++iter;
        }
        value = std::string(pStr, iter);
        pEnd = const_cast<char*>(iter);
    }

    template<typename ValueType>
    ParseResult Parse(const char*& pStr, ValueType& value)
    { 
        // trim whitespace
        Trim(pStr);

        // check for eof
        if(*pStr == '\0')
        {
            return ParseResult::endOfString;
        }

        // check for "//" comment indicator
        if(*pStr == '/')
        {
            if(*(pStr+1) == '/')
            {
                return ParseResult::beginComment;
            }
        }

        // check for "#" comment indicator
        if(*pStr == '#')
        {
            return ParseResult::beginComment;
        }

        char* pEnd = 0;

        // record error state
        int state = errno;
        errno = 0;

        // try to parse
        cParse(pStr, pEnd, value);
        
        // check for parse errors
        if(pStr == pEnd)
        {
            return ParseResult::badFormat;
        }
        if(errno == ERANGE)
        {
            return ParseResult::outOfRange;
        }

        // restore state
        errno = state;

        // increment pointer
        pStr = pEnd; 

        return ParseResult::success;
    }

    void Trim(const char*& pStr)
    {
        while (std::isspace(*pStr))
        {
            ++pStr;
        }
    }
}
