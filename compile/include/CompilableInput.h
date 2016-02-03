// CompilableInput.h

#pragma once

#include "CompilableLayer.h"

#include "Input.h"

/// A struct that adds printing capabilities to a layer
///
struct CompilableInput : public layers::Input, public CompilableLayer
{
    virtual void BackwardPass(uint64 currentLayerIndex, vector<vector<vector<AddToAction>>>& actions) const override;
};
