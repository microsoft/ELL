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
    /// This function is called for each layer, from the bottom up
    ///
    virtual void SetActions(uint64 currentLayerIndex, DataFlowGraph& graph) const = 0;

    //virtual void ProcessForward() = 0;
};

 