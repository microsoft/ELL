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
    ValueSelectorNode(NodeOutput<bool> condition, NodeOutput<ValueType> values): Node({condition, values}, {&output}), output(this, 1) {};

    NodeOutput<ValueType> output;
};
