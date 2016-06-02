#pragma once
//
// ArgMaxNode
// 

#include "Node.h"
#include "NodeInput.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>

template <typename ValueType, bool max>
class ExtremalValNode: public Node
{
public:
    ExtremalValNode(NodeInput<ValueType> values);

    NodeOutput<ValueType> extremalVal;
    NodeOutput<int> argExtremalVal;
};

template <typename ValueType>
using ArgMinNode = ExtremalValNode<ValueType, false>;

template <typename ValueType>
using ArgMaxNode = ExtremalValNode<ValueType, true>;
