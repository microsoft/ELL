// compilable.cpp

#pragma once

#include "mappings.h"
#include <iostream>

using std::ostream;
using namespace mappings;

namespace compile
{
    /// An interface for classes that can compile human-friendly descriptions of themselves
    struct compilable 
    {
        /// compiles a human-firiendly description of the underlying class to an output stream
        ///
        virtual void compile(ostream& os, int indentation = 0, int enumeration = 0) = 0;
    };
}