////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StringFormat.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <iostream>
#include <iomanip>

namespace utilities
{
    template<typename ArgType, typename ... ArgTypes>
    void StringFormat(std::ostream& os, const char* cstr, ArgType arg, ArgTypes ...args)
    {
        int n=0;
        const char* ptr = cstr;
        while(*ptr != '%' && *ptr != '\0')
        {
            ++n;
            ++ptr;
        }

        os.write(cstr, n);

        // if reached end of string, exit
        if(*ptr == '\0')
        {
            return;
        }

        // if reached '%' character, print an arg
        char specifier = *(++ptr);
        auto prec = os.precision();

        switch(specifier)
        {
        case 'i':
            os << std::fixed << std::setprecision(0) << arg << std::setprecision(prec);
            break;

        case 'f':
            os << std::fixed << arg;
            break;

        case 'e':
            os << std::scientific << arg;
            break;

        case 's':
            os << std::defaultfloat << arg;
            break;

        case '%':
            os << '%';
            break;
        }

        // if end of string reached, exit
        ++ptr;
        if(*ptr == '\0')
        {
            return;
        }

        StringFormat(os, ptr, args...);
    }

    template<typename ... ArgTypes>
    void StringFormat(std::ostream& os, const char* cstr, ArgTypes ...args)
    {
        os << cstr;
    }


}
