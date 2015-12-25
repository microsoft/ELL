// CompilableScale.h

#pragma once

#include "Compilable.h"

#include "layers.h"
using namespace layers;

/// A struct that adds printing capabilities to a layer
///
struct CompilableScale : public Scale, public Compilable
{
public:
    /// Returns the number of elements in the layer
    ///
    virtual uint64 Size() const override;
};