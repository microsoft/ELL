// CompilableSum.h

#pragma once

#include "CompilableLayer.h"

#include "Sum.h"
using layers::Sum;

/// A struct that adds printing capabilities to a layer
///
struct CompilableSum : public Sum, public CompilableLayer
{
public:
    virtual void BackwardPass(uint64 currentLayerIndex, vector<vector<vector<AddToAction>>>& actions) const override;
};