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
    ValueSelectorNode(NodeOutput<bool> condition, NodeOutput<ValueType> value1, NodeOutput<ValueType> value2): Node({condition, value1, value2}, {&output}), output(this, 1) {};

    NodeOutput<ValueType> output;
};
