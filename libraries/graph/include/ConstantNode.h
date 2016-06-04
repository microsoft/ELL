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
    ConstantNode(ValueType value) : Node({}, {&_output}), _output(this, 0, 1), _values({value}) {};
    ConstantNode(std::vector<ValueType> values) : Node({&_output}), _output(this, values.size()), _values(values) {};

    virtual std::string NodeType() const override { return "Constant"; }

    const NodeOutput<ValueType>& output = _output;

private:
    std::vector<ValueType> _values;
    
    NodeOutput<ValueType> _output;
};
