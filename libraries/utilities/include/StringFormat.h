// StringFormat.h

#pragma once

#include "types.h"

// stl
#include <string>
using std::string;

namespace utilities
{
    ///
    ///
    template<typename ArgType, typename ... ArgTypes>
    void StringFormat(ostream& os, const char* cstr, ArgType arg, ArgTypes ...args);

    ///
    ///
    template<typename ... ArgTypes>
    void StringFormat(ostream& os, const char* cstr, ArgTypes ...args);
}

#include "../tcc/StringFormat.tcc"
