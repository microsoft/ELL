#pragma once
//
// ValueSelectorNode
// 

#include "Node.h"
#include "NodeInput.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>

template <typename ValueType>
class ValueSelectorNode : public Node
{
public:
    ValueSelectorNode(NodeOutput<bool> condition, NodeOutput<ValueType> value1, NodeOutput<ValueType> value2): Node({&_condition, &_value1, &_value2}, {&_output}), _condition(condition), _value1(value1), _value2(value2), _output(this, 0, 1) {};

    virtual std::string NodeType() const override { return "ValueSelector"; }

    // Output
    const NodeOutput<ValueType>& output = _output;

private:
    // Inputs
    NodeInput _condition;
    NodeInput _value1;
    NodeInput _value2;
    
    // Output
    NodeOutput<ValueType> _output;
};
