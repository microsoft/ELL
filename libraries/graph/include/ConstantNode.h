#pragma once
//
// ConstantNode
// 

#include "Node.h"
#include "NodeOutput.h"
#include "NodeInput.h"

#include <vector>
#include <memory>

template <typename ValueType>
class ConstantNode : public Node
{
public:
    ConstantNode(ValueType value) : Node({}, {&_output}), _output(this, 0, 1), _values({value}) {};
    ConstantNode(std::vector<ValueType> values) : Node({&_output}), _output(this, values.size()), _values(values) {};

    virtual std::string Type() const override { return "Constant"; }

    NodeOutput<ValueType> _output;

private:
    std::vector<ValueType> _values;
};
