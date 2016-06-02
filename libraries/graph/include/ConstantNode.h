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
    ConstantNode(ValueType);

    NodeOutput<ValueType> output;

private:
    std::vector<ValueType> _values;
};
