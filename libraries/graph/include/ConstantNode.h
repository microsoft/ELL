#pragma once
//
// ConstantNode
// 

#include "Node.h"
#include "NodeOutput.h"
#include "NodeInput.h"

#include <vector>
#include <memory>

//
// ConstantNode: A node that contains a constant value. Has no inputs.
//
template <typename ValueType>
class ConstantNode : public Node
{
public:
    // Constructor for a scalar constant
    ConstantNode(ValueType value);

    // Constructor for a vector constant
    ConstantNode(std::vector<ValueType> values);

    // Returns the name for this node class
    virtual std::string NodeType() const override { return "Constant"; }

    // This is a somewhat cringe-inducing way to make the equivalent of a read-only property in C++
    const NodeOutput<ValueType>& output = _output;

protected:
    virtual void ComputeOutput() const override;

private:
    std::vector<ValueType> _values;
    
    NodeOutput<ValueType> _output;
};

#include "../tcc/ConstantNode.tcc"
