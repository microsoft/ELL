#pragma once

#include "Node.h"

#include <cstddef>
#include <memory>

// We could have 2 types of ports and 2 types of edges: input and output
// I suggest we don't, because we can enforce that edges point the right way by construction (?)
// Having a simpler model is better.
class Edge
{
public:
    std::shared_ptr<Node> GetNode() { return _destinationNode; }
    const std::shared_ptr<Node> GetNode() const { return _destinationNode; }
    Node::Port::Id GetPort() { return _destinationPort; }
    
private:
    std::shared_ptr<Node> _destinationNode;
    Node::Port::Id _destinationPort;

    // Alternately, we could have pointers to ports, and the port would have a pointer/reference to the node
    // The choice depends on what dependencies we care most about: nodes-nodes or ports-ports. If we care about ports,
    // then a node is basically equivalent to a hierarchical node or a central "hub" node with sattelite ports. 
    // Node::Port *_destination;    
};
