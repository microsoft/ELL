// parsing.tcc

#include "types.h"
using linear::uint;

#include <cstdlib>
using std::strtod;

#include <stdexcept>
using std::runtime_error;

#include<string>
using std::string;

namespace dataset
{
    // wrapper for strtof
    inline void cParse(const char*& pStr, char*& pEnd, float& value)
    {
        value = strtof(pStr, &pEnd);
    }

    // wrapper for strtod
    inline void cParse(const char*& pStr, char*& pEnd, double& value)
    {
        value = strtod(pStr, &pEnd);
    }

    // wrapper for stroul
    inline void cParse(const char*& pStr, char*& pEnd, unsigned int& value)
    {
        unsigned long x = strtoul(pStr, &pEnd, 0);
        if(x != (unsigned int)x)
        {
            errno = ERANGE;
        }
        value = (unsigned int)x;
    }

    // wrapper for strtoul
    inline void cParse(const char*& pStr, char*& pEnd, uint& value)
    {
        unsigned long x = strtoul(pStr, &pEnd, 0);
        if(x != (uint)x)
        {
            errno = ERANGE;
        }
        value = (uint)x;
    }

    // wrapper for strtol
    inline void cParse(const char*& pStr, char*& pEnd, int& value)
    {
        long x = strtol(pStr, &pEnd, 0);
        if(x != (int)x)
        {
            errno = ERANGE;
        }
        value = (int)x;
    }

    // wrapper for strtoul
    inline void cParse(const char*& pStr, char*& pEnd, unsigned long& value)
    {
        value = strtoul(pStr, &pEnd, 0);
    }

    // wrapper for strtol
    inline void cParse(const char*& pStr, char*& pEnd, long& value)
    {
        value = strtol(pStr, &pEnd, 0);
    }

    // wrapper for strtoul
    inline void cParse(const char*& pStr, char*& pEnd, unsigned short& value)
    {
        unsigned long x = strtoul(pStr, &pEnd, 0);
        if(x != (unsigned short)x)
        {
            errno = ERANGE;
        }
        value = (unsigned short)x;
    }

    // wrapper for strtol
    inline void cParse(const char*& pStr, char*& pEnd, short& value)
    {
        long x = strtol(pStr, &pEnd, 0);
        if(x != (short)x)
        {
            errno = ERANGE;
        }
        value = (short)x;
    }

    template<typename ValueType>
    ParseResults parse(const char*& pStr, /* out */ ValueType& value)
    { 
        // trim whitespace
        trim(pStr);

        // check for eof
        if(*pStr == '\0')
        {
            return ParseResults::endOfString;
        }

        // check for "//" comment indicator
        if(*pStr == '/')
        {
            if(*(pStr+1) == '/')
            {
                return ParseResults::beginComment;
            }
        }

        // check for "#" comment indicator
        if(*pStr == '#')
        {
            return ParseResults::beginComment;
        }

        char* pEnd = 0;

        // record error state
        int state = errno;
        errno = 0;

        // try to parse
        cParse(pStr, pEnd, /* out */ value);
        
        // check for parse errors
        if(pStr == pEnd)
        {
            return ParseResults::badFormat;
        }
        if(errno == ERANGE)
        {
            return ParseResults::outOfRange;
        }

        // restore state
        errno = state;

        // increment pointer
        pStr = pEnd; 

        return ParseResults::success;
    }

    inline void trim(const char*& pStr)
    {
        while(isspace(*pStr))
        {
            ++pStr;
        }
    }
}