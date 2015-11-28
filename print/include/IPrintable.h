// IPrintable.h

#pragma once

#include "layers.h"
using namespace layers;

#include <iostream>
using std::ostream;

/// An interface for classes that can print human-friendly descriptions of themselves
struct IPrintable 
{
    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual void Print(ostream& os, int indentation = 0, int enumeration = 0) = 0;
};

