////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DataFlowGraph.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataFlowNode.h"

// types
#include "types.h"

// stl
#include <vector>

class DataFlowGraph : public std::vector<std::vector<DataFlowNode>>
{
public:

    /// <summary> Adds a layer to the data flow graph </summary>
    ///
    /// <param name="numNodes"> Number of nodes in the layer
    ///                         . </param>
    void AddLayer(uint64 numNodes);

    // TODO add operator[] with coordinate
};