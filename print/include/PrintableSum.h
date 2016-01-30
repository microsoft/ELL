// PrintableSum.h

#pragma once

#include "PrintableLayer.h"
#include "LayerLayout.h"

#include "Sum.h"
using layers::Sum;

/// A struct that adds printing capabilities to a layer
///
struct PrintableSum : public Sum, public PrintableLayer
{
public:
    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual LayerLayout Print(ostream& os, double left, double top, uint64 layerIndex, const CommandLineArguments& args) const override;
};