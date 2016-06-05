#pragma once
//
// ExtremalValNode
// 

#include "Node.h"
#include "NodeInput.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>

template <typename ValueType, bool max>
class ExtremalValueNode: public Node
{
public:
    ExtremalValueNode(NodeOutput<ValueType> input);

    virtual std::string NodeType() const override { return max ? "ArgMax" : "ArgMin"; }

    // read-only properties
    const NodeOutput<ValueType>& val = _val;
    const NodeOutput<int>& argVal = _argVal;

protected:
    virtual void ComputeOutput() const override;
    
private:
    // My inputs
    NodeInput _input;
    
    // My outputs
    NodeOutput<ValueType> _val;
    NodeOutput<int> _argVal;
};

template <typename ValueType>
using ArgMinNode = ExtremalValueNode<ValueType, false>;

template <typename ValueType>
using ArgMaxNode = ExtremalValueNode<ValueType, true>;

#include "../tcc/ExtremalValueNode.tcc"
