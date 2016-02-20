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

class DataFlowGraph 
{
public:

    /// <summary> Adds a layer to the data flow graph </summary>
    ///
    /// <param name="numNodes"> Number of nodes in the layer
    ///                         . </param>
    void AddLayer(uint64 numNodes);

    const DataFlowNode& GetNode(layers::Coordinate coordinate) const;

    DataFlowNode& GetNode(layers::Coordinate coordinate);

    const DataFlowNode& GetNode(uint64 layerIndex, uint64 elementIndex) const;

    DataFlowNode& GetNode(uint64 layerIndex, uint64 elementIndex);

private:
    std::vector<std::vector<DataFlowNode>> _nodes;
};