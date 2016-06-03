#pragma once
//
// ExtremalValNode
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
    ExtremalValNode(NodeOutput<ValueType> input) : Node({input}, {&val, &argVal}), val(this, 1), argVal(this, 1) {};



    // My inputs
    NodeInput input;
    
    // My outputs
    NodeOutput<ValueType> val;
    NodeOutput<int> argVal;
};

template <typename ValueType>
using ArgMinNode = ExtremalValNode<ValueType, false>;

template <typename ValueType>
using ArgMaxNode = ExtremalValNode<ValueType, true>;
