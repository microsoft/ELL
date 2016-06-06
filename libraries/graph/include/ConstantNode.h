#pragma once
//
// ConstantNode
// 

#include "Node.h"
#include "OutputPort.h"
#include "InputPort.h"

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
    virtual std::string GetTypeName() const override { return "Constant"; }

    // Exposing the output port as a read-only property
    const OutputPort<ValueType>& output = _output;

protected:
    virtual void Compute() const override;

private:
    std::vector<ValueType> _values;    
    OutputPort<ValueType> _output;
};

#include "../tcc/ConstantNode.tcc"
