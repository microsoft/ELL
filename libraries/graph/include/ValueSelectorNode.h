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
    ValueSelectorNode(NodeInput<bool> condition, NodeInput<ValueType> values);

    NodeOutput<ValueType> output;
};
