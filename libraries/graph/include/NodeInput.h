#pragma once

//
// NodeInput
// 
#include "NodeOutput.h"

#include <vector>
#include <memory>

class NodeInputBase
{
public:
    NodeInputBase(const NodeOutputBase& output): _node(output.GetNode()), _outputIndex(output.GetOutputIndex()) {}
    const Node* GetNode() const { return _node; }
    size_t GetOutputIndex() const { return _outputIndex; }

protected:
    
private:
    // keeps info on where the input is coming from (which "port")

    const Node* _node = nullptr;
    size_t _outputIndex = 0;
    // optional offset, size
    // maybe a union?
    // Later, maybe keep a list of (node*, outputIndex, start, len) tuples    
};

class NodeInput : public NodeInputBase
{
public:
    template <typename ValueType>
    NodeInput(const NodeOutput<ValueType>& output): NodeInputBase(output) {};

    // "concat" version
    // NodeInput(const std::vector<NodeOutput<ValueType>>& outputs);
};

// If we insist on a complicated interface for crazy swizzled edges, NodeInput should be the class that has that interface 
