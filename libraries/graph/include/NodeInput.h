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

//
// NodeInput: each node/function stores a list of NodeInput objects that represent the inputs to the computation
//
class NodeInput : public NodeEdge
{
public:
    // constructor --- takes the output that we take our value from
    template <typename ValueType>
    NodeInput(const NodeOutput<ValueType>& output);

    // "concat" version
    // NodeInput(const std::vector<NodeOutput<ValueType>>& outputs);

    // returns the (already-computed) output value corresponding to this input    
    template <typename ValueType>
    std::vector<ValueType> GetValue() const;
    
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