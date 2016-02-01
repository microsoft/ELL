// PrintableLayer.h

#pragma once

#include "LayerLayout.h"
#include "PrintArguments.h"

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
    virtual LayerLayout Print(ostream& os, double left, double top, uint64 layerIndex, const PrintArguments& Arguments) const = 0;

protected:
    LayerLayout Print(ostream& os, double left, double top, uint64 layerIndex, const string& typeName, uint64 size, const ElementLayoutArguments& elementLayout, const LayerStyleArguments& layerStyle) const;
};

 