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

/// <summary> A data flow graph. </summary>
class DataFlowGraph 
{
public:

    /// <summary> Adds a layer to the data flow graph </summary>
    ///
    /// <param name="numNodes"> Number of nodes in the layer</param>
    void AddLayer(uint64 numNodes);

    /// <summary> Gets the node at a specified coordinate. </summary>
    ///
    /// <param name="coordinate"> The coordinate of the requested node. </param>
    ///
    /// <returns> The node. </returns>
    const DataFlowNode& GetNode(layers::Coordinate coordinate) const;

    /// <summary> Gets a node at a specified coordinate. </summary>
    ///
    /// <param name="coordinate"> The coordinate of the requested node. </param>
    ///
    /// <returns> The node. </returns>
    DataFlowNode& GetNode(layers::Coordinate coordinate);

    /// <summary> Gets a node at a specified coordinate. </summary>
    ///
    /// <param name="layerIndex"> Zero-based index of the layer. </param>
    /// <param name="elementIndex"> Zero-based index of the element. </param>
    ///
    /// <returns> The node. </returns>
    const DataFlowNode& GetNode(uint64 layerIndex, uint64 elementIndex) const;

    /// <summary> Gets a node at a specified coordinate. </summary>
    ///
    /// <param name="layerIndex"> Zero-based index of the layer. </param>
    /// <param name="elementIndex"> Zero-based index of the element. </param>
    ///
    /// <returns> The node. </returns>
    DataFlowNode& GetNode(uint64 layerIndex, uint64 elementIndex);

private:
    std::vector<std::vector<DataFlowNode>> _nodes;
};