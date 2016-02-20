////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DataFlowGraph.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataFlowGraph.h"

void DataFlowGraph::AddLayer(uint64 numNodes)
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

const DataFlowNode& DataFlowGraph::GetNode(uint64 layerIndex, uint64 elementIndex) const
{
    return _nodes[layerIndex][elementIndex];
}

DataFlowNode& DataFlowGraph::GetNode(uint64 layerIndex, uint64 elementIndex)
{
    return _nodes[layerIndex][elementIndex];
}
