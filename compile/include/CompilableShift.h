// CompilableShift.h

#pragma once

#include "CompilableLayer.h"

#include "Shift.h"
using layers::Shift;

/// A struct that adds printing capabilities to a layer
///
struct CompilableShift : public Shift, public CompilableLayer
{
public:
    virtual void BackwardPass(uint64 currentLayerIndex, vector<vector<vector<AddToAction>>>& actions) const override;
};