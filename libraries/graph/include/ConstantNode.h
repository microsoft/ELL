#pragma once
//
// ConstantNode
// 

#include "Node.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>

template <typename ValueType>
class ConstantNode : public Node
{
public:
    ConstantNode(ValueType value) : Node({}, {&output}), output(this, 1), _values({value}) {};
    ConstantNode(std::vector<ValueType> values) : Node({&output}), output(this, values.size()), _values(values) {};

    NodeOutput<ValueType> output;

private:
    std::vector<ValueType> _values;
};
