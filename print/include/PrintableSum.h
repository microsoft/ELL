// PrintableSum.h

#pragma once

#include "PrintableEmpty.h"
#include "HorizontalLayout.h"

#include "layers.h"
using namespace layers;

/// A struct that adds printing capabilities to a layer
///
struct PrintableSum : public Sum, public PrintableEmpty
{
public:
    /// Returns the number of elements in the layer
    ///
    virtual uint64 Size() const override;

    /// \returns the layer type name
    ///
    virtual string GetTypeName() const override;
};