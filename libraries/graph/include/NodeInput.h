#pragma once

//
// NodeInput
// 
#include "NodeOutput.h"

#include <vector>
#include <memory>

class NodeInput
{
public:
    template <typename ValueType>
    NodeInput(const NodeOutput<ValueType>& output);

    // "concat" version
    // NodeInput(const std::vector<NodeOutput<ValueType>>& outputs);

private:
    // keeps info on where the input is coming from (which "port")
    // Node *node
    // size_t index
    // or something like that  
};

// If we insist on a complicated interface for crazy swizzled edges, NodeInput should be the class that has that interface 
