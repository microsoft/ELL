////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableInput.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableLayer.h"

#include "Input.h"

/// A struct that adds printing capabilities to a layer
///
struct CompilableInput : public layers::Input, public CompilableLayer
{
    virtual void BackwardPass(uint64 currentLayerIndex, DataFlowGraph& graph) const override;
};
