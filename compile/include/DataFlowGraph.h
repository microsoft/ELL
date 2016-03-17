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

// stl
#include <cstdint>
#include <vector>

/// <summary> A data flow graph. </summary>
class DataFlowGraph 
{
public:

    /// <summary> Adds a layer to the data flow graph </summary>
    ///
    /// <param name="numNodes"> Number of nodes in the layer</param>
    void AddLayer(uint64_t numNodes);

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
    const DataFlowNode& GetNode(uint64_t layerIndex, uint64_t elementIndex) const;

    /// <summary> Gets a node at a specified coordinate. </summary>
    ///
    /// <param name="layerIndex"> Zero-based index of the layer. </param>
    /// <param name="elementIndex"> Zero-based index of the element. </param>
    ///
    /// <returns> The node. </returns>
    DataFlowNode& GetNode(uint64_t layerIndex, uint64_t elementIndex);

private:
    std::vector<std::vector<DataFlowNode>> _nodes;
};