// CompilableSum.h

#pragma once

#include "CompilableLayer.h"

// layers
#include "Sum.h"

/// A struct that adds printing capabilities to a layer
///
struct CompilableSum : public layers::Sum, public CompilableLayer
{
public:
    virtual void BackwardPass(uint64 currentLayerIndex, vector<vector<vector<AddToAction>>>& actions) const override;
};