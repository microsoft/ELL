////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableCoordinatewise.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilableLayer.h"

// layers
#include "Coordinatewise.h"

/// <summary> A struct that adds printing capabilities to a layer. </summary>
class CompilableCoordinatewise : public layers::Coordinatewise, public CompilableLayer
{
public:

    CompilableCoordinatewise(layers::Layer::Type type);

    /// <summary> Pushes actions upward in the graph </summary>
    ///
    /// <param name="currentLayerIndex"> The index of the current layer. </param>
    /// <param name="graph"> [in,out] The data flow graph. </param>
    virtual void SetActions(uint64 currentLayerIndex, DataFlowGraph& graph) const override;
};