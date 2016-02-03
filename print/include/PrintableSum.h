// PrintableSum.h

#pragma once

#include "PrintableLayer.h"
#include "LayerLayout.h"

// layers
#include "Sum.h"

/// A struct that adds printing capabilities to a layer
///
struct PrintableSum : public layers::Sum, public PrintableLayer
{
public:
    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual LayerLayout Print(ostream& os, double left, double top, uint64 layerIndex, const PrintArguments& Arguments) const override;
};