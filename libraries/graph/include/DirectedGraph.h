#pragma once

#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

#include <unordered_set>
#include <vector>
#include <memory>
#include <unordered_map>
#include <exception>

//
// DirectedGraph
//
class DirectedGraph
{
public:

    // AddNode is the factory method used to create nodes and add them to the graph.
    template <typename NodeType, typename... Args>
    std::shared_ptr<NodeType> AddNode(Args... args);
    
    // `GetNode()` looks up a node by id. Returns nullptr if node not found. Maybe should be called `FindNode()`
    // Q: should this return a Node*, a weak_ptr<Node>, or a shared_ptr<Node>?
    // Node* -- pro: easy to use. con: potentially less safe (but not really)
    // weak_ptr<Node> -- pro: potentially more safe. con: fussy to use. Potentially semantically incorrect
    // shared_ptr<Node> --- pro: easier to use than weak_ptr, con: wrong
    std::weak_ptr<Node> GetNode(Node::NodeId id);

    // `GetNodeOutput` -- Computes and returns the computed output value for a node
    // There are 2 overloads for `GetNodeOutput`. One takes a typed OutputPort<T>, and its output type is compile-time enforced. The other
    // takes a node and index into its outputs, and requires the user to specify the output type as a template parameter. We must check
    // that the types are compatible at runtime.
    template <typename ValueType>
    std::vector<ValueType> GetNodeOutput(const std::shared_ptr<Node>& node, size_t outputIndex) const;
    
    template <typename ValueType>
    std::vector<ValueType> GetNodeOutput(const OutputPort<ValueType>& OutputPort) const;
    
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
    // TODO begin/end for iterating over entire graph
    
private:
    // The id->node map acts both as the main container that holds the shared pointers to nodes, and as the index
    // to look nodes up by id.
    std::unordered_map<Node::NodeId, std::shared_ptr<Node>> _nodeMap;

    // This is the single implementation of Visit, invoked by the publicly-visible ones
    template <typename Visitor>
    void Visit(Visitor& visitor, const std::vector<const Node*>& outputNodePtrs) const;
};

#include "../tcc/DirectedGraph.tcc"
