////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableShift.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableLayer.h"

// layers
#include "Shift.h"

/// A struct that adds printing capabilities to a layer
///
struct CompilableShift : public layers::Shift, public CompilableLayer
{
public:
    virtual void BackwardPass(uint64 currentLayerIndex, DataFlowGraph& graph) const override;
};