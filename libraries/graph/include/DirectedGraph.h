#pragma once

#include <vector>
#include <ostream>

// Graph.h
class Node;

// "NodeRef" here is just a name for whatever we use as a pointer to a node. Maybe it's a referece, maybe a pointer
typedef Node& NodeRef;

class DirectedGraph
{
public:
    
    // CreateNode
    
    template <typename Visitor>
    void Visit(Visitor& visitor) const; // Visits all nodes in the graph

    template <typename Visitor>
    void Visit(const NodeRef outputNode, Visitor& visitor) const; // Visits all nodes in the graph necssary to compute outputNode

    template <typename Visitor>
    void Visit(const std::vector<NodeRef>& outputNode, Visitor& visitor) const; // Visits all nodes in the graph necssary to compute all outputNodes
    
private:
    // the nodes (?)
};