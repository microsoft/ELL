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
class CompilableShift : public layers::Shift, public CompilableLayer
{
public:
    /// <summary> Pushes actions upward in the graph </summary>
    ///
    /// <param name="currentLayerIndex"> The index of the current layer. </param>
    /// <param name="graph"> [in,out] The data flow graph. </param>
    virtual void SetActions(uint64 currentLayerIndex, DataFlowGraph& graph) const override;
};