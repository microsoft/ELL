// StringFormat.h

#pragma once

#include "types.h"

// stl
#include <string>
using std::string;

namespace utilities
{
    template<typename ... Args>
    string StringFormat(const string& input, Args ... args);
}

#include "../tcc/StringFormat.tcc"
