#pragma once

#include "Node.h"
#include "NodeInput.h"
#include "NodeOutput.h"

#include <unordered_set>
#include <vector>
#include <memory>
#include <unordered_map>

//
// DirectedGraph
//
class DirectedGraph
{
public:

    // AddNode is the factory method used to create nodes and add them to the graph.
    template <typename NodeType, typename... Args>
    std::shared_ptr<NodeType> AddNode(Args... args);
    
    // GetNode looks up a node by id. Returns nullptr if node not found.
    // Q: should this return a Node*, a weak_ptr<Node>, or a shared_ptr<Node>?
    // Node* -- pro: easy to use. con: potentially less safe (but not really)
    // weak_ptr<Node> -- pro: potentially more safe. con: fussy to use. Potentially semantically incorrect
    // shared_ptr<Node> --- pro: easier to use than weak_ptr, con: wrong
    std::weak_ptr<Node> GetNode(Node::NodeId id);

    // GetNodeOutput -- Computes and returns the computed output value for a node
    template <typename ValueType>
    std::vector<ValueType> GetNodeOutput(const std::shared_ptr<Node>& node, size_t outputIndex) const;
    
    template <typename ValueType>
    std::vector<ValueType> GetNodeOutput(const NodeOutput<ValueType>& nodeOutput) const;
    
    // Visitors
    // The visitor functions visit the nodes in the graph in dependency order. No nodes
    // will be visited until all their inputs have first been visited. 
    
    // This visitor (without any `Node` arguments) visits all nodes in the graph
    template <typename Visitor>
    void Visit(Visitor& visitor) const;

    // These two visitors only visit the nodes in the graph necessary to compute the output of the given node(s)
    template <typename Visitor>
    void Visit(Visitor& visitor, const std::shared_ptr<Node>& outputNode) const;

    template <typename Visitor>
    void Visit(Visitor& visitor, const std::vector<std::shared_ptr<Node>>& outputNodes) const;
    
    // TODO: iterators
    
private:
    // The node map acts both as the main container that holds the shared pointers to nodes, and as the index
    // to look nodes up by id.
    std::unordered_map<Node::NodeId, std::shared_ptr<Node>> _nodeMap;

    template <typename Visitor>
    void Visit(Visitor& visitor, const std::vector<const Node*>& outputNodePtrs) const;
};

#include "../tcc/DirectedGraph.tcc"
