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
    template <typename ValueType>
    NodeInput(const NodeOutput<ValueType>& output);

    // "concat" version
    // NodeInput(const std::vector<NodeOutput<ValueType>>& outputs);
    
    template <typename ValueType>
    std::vector<ValueType> GetValue() const;
    
protected:
    
private:
    // optional offset, size
    // maybe a union?
    // Later, maybe keep a list of     
};

// TODO: if we want to allow functions/nodes to gather values from arbitrary collections of output elements, then
//       we'll probablay want to make NodeInput not be a subclass of NodeEdge, but for it to contain a vector of
//       (Node*, outputIndex, start, len) items.
//       We'll have to replace `GetNode()` with `GetNodes()` and maybe just remove `OutputIndex()`.

#include "../tcc/NodeInput.tcc"