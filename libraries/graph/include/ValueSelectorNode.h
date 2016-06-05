#pragma once
//
// ValueSelectorNode
// 

#include "Node.h"
#include "NodeInput.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>
#include <exception>

template <typename ValueType>
class ValueSelectorNode : public Node
{
public:
    ValueSelectorNode(NodeOutput<bool> condition, NodeOutput<ValueType> value1, NodeOutput<ValueType> value2);

    virtual std::string NodeType() const override { return "ValueSelector"; }

    // Output
    const NodeOutput<ValueType>& output = _output;

protected:
    virtual void ComputeOutput() const override;

private:
    // Inputs
    NodeInput _condition;
    NodeInput _value1;
    NodeInput _value2;
    
    // Output
    NodeOutput<ValueType> _output;
};

#include "../tcc/ValueSelectorNode.tcc"
