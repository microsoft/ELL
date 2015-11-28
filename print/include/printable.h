// printable.cpp

#pragma once

#include "layers.h"
#include <iostream>

using std::ostream;
using namespace layers;

namespace Print
{
    /// An interface for classes that can print human-friendly descriptions of themselves
    struct printable 
    {
        /// Prints a human-firiendly description of the underlying class to an output stream
        ///
        virtual void Print(ostream& os, int indentation = 0, int enumeration = 0) = 0;
    };
}