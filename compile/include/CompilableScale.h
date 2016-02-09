////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableScale.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableLayer.h"

// layers
#include "Scale.h"

/// A struct that adds printing capabilities to a layer
///
struct CompilableScale : public layers::Scale, public CompilableLayer
{
public:
    virtual void BackwardPass(uint64 currentLayerIndex, vector<vector<vector<AddToAction>>>& actions) const override;
};