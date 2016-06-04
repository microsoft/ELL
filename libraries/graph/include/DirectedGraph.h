#pragma once

#include "Node.h"

#include <vector>
#include <ostream>
#include <memory>

// Graph.h

class DirectedGraph
{
public:
    template <typename NodeType, typename... Args>
    std::shared_ptr<NodeType> AddNode(Args... args);
    
    template <typename Visitor>
    void Visit(Visitor& visitor) const; // Visits all nodes in the graph

    template <typename Visitor>
    void Visit(Visitor& visitor, const std::shared_ptr<Node>& outputNode) const; // Visits all nodes in the graph necssary to compute outputNode

    template <typename Visitor>
    void Visit(Visitor& visitor, const std::vector<std::shared_ptr<Node>>& outputNode) const; // Visits all nodes in the graph necssary to compute all outputNodes
    
private:
    // std::unordered_map<NodeId, std::shared_ptr<Node>> _nodeMap;
};

#include "../tcc/DirectedGraph.tcc"