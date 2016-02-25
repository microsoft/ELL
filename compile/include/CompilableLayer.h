////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableLayer.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataFlowGraph.h"

// types
#include "types.h"

// stl
#include <vector>
#include <string>

/// An interface for classes that can print human-friendly descriptions of themselves
///
class CompilableLayer 
{
public:
    /// <summary> Pushes actions upward in the graph </summary>
    ///
    /// <param name="currentLayerIndex"> The index of the current layer. </param>
    /// <param name="graph"> [in,out] The data flow graph. </param>
    virtual void SetActions(uint64 currentLayerIndex, DataFlowGraph& graph) const = 0;
};

 