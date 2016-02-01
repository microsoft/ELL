// PrintableLayer.h

#pragma once

#include "LayerLayout.h"
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
class PrintableLayer 
{
public:
    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual LayerLayout Print(ostream& os, double left, double top, uint64 layerIndex, const CommandLineArguments& args) const = 0;

protected:
    LayerLayout Print(ostream& os, double left, double top, uint64 layerIndex, const string& typeName, uint64 size, double maxWidth, const ElementLayoutArgs& elementLayout, const LayerStyleArgs& layerStyle) const;
};

 