// Compilable.h

#pragma once

#include "CommandLineArguments.h"

#include "layers.h"
using namespace layers;

#include "types.h"

#include <iostream>
using std::ostream;

#include <memory>
using std::shared_ptr;

/// An interface for classes that can print human-friendly descriptions of themselves
///
class Compilable 
{
public:
    /// Returns the number of elements in the layer
    ///
    virtual uint64 Size() const = 0;
};

 