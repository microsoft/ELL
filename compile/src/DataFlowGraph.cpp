////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     DataFlowGraph.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataFlowGraph.h"

void DataFlowGraph::AddLayer(uint64_t numNodes)
{
    _nodes.emplace_back(numNodes);
}

const DataFlowNode& DataFlowGraph::GetNode(layers::Coordinate coordinate) const
{
    return _nodes[coordinate.GetLayerIndex()][coordinate.GetElementIndex()];
}

DataFlowNode& DataFlowGraph::GetNode(layers::Coordinate coordinate)
{
    return _nodes[coordinate.GetLayerIndex()][coordinate.GetElementIndex()];
}

const DataFlowNode& DataFlowGraph::GetNode(uint64_t layerIndex, uint64_t elementIndex) const
{
    return _nodes[layerIndex][elementIndex];
}

DataFlowNode& DataFlowGraph::GetNode(uint64_t layerIndex, uint64_t elementIndex)
{
    return _nodes[layerIndex][elementIndex];
}
