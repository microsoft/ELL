// PrintableInput.h

#pragma once

#include "PrintableLayer.h"
#include "LayerLayout.h"

#include "Input.h"
using layers::Input;

/// A struct that adds printing capabilities to a layer
///
struct PrintableInput : public Input, public PrintableLayer
{
public:
    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual LayerLayout Print(ostream& os, double left, double top, uint64 layerIndex, const CommandLineArguments& args) const override;
};