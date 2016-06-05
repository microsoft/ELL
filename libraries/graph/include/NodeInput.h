#pragma once

//
// NodeInput
// 
#include "NodeEdge.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>
#include <cassert>

class Node;

class NodeInput : public NodeEdge
{
public:
//    NodeInput(const NodeEdge& output): _node(output.GetNode()), _outputIndex(output.GetOutputIndex()) {}

    template <typename ValueType>
    NodeInput(const NodeOutput<ValueType>& output);

    // "concat" version
    // NodeInput(const std::vector<NodeOutput<ValueType>>& outputs);

    NodeEdge::OutputType GetType() const;
    
    template <typename ValueType>
    std::vector<ValueType> GetValue() const;
    
protected:
    
private:
    // optional offset, size
    // maybe a union?
    // Later, maybe keep a list of (node*, outputIndex, start, len) tuples    
};

// If we have a class for crazy swizzled edges, NodeInput should be the class that has that interface 
// Idea: have NodeInput be base class for NodeOutput (rename appropriately). Then the NodeInput(vector<...>) constructor can be a vector of NodeInputs.
//       ...then concat and subset operators can return NodeInputs, which is what we'd want them to return 

#include "../tcc/NodeInput.tcc"