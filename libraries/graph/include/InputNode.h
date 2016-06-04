#pragma once
//
// InputNode
// 

#include "Node.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>
#include <string>

template <typename ValueType>
class InputNode: public Node
{
public:
    InputNode(size_t dimension);
    void SetInput(std::vector<ValueType> inputValues);
    
    virtual std::string NodeType() const override { return "Input"; }

    const NodeOutput<ValueType>& output = _output; // This is a (perhaps too-)clever way to make a read-only property. I don't know if I like it.

private:
    NodeOutput<ValueType> _output;
};

#include "../tcc/InputNode.tcc"
