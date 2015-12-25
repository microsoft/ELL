// CompilableShift.h

#pragma once

#include "Compilable.h"

#include "layers.h"
using namespace layers;

/// A struct that adds printing capabilities to a layer
///
struct CompilableShift : public Shift, public Compilable
{
public:
    /// Returns the number of elements in the layer
    ///
    virtual uint64 Size() const override;
};