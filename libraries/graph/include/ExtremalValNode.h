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
    ExtremalValNode(NodeOutput<ValueType> input) : Node({&_input}, {&_val, &_argVal}), _val(this, 0, 1), _argVal(this, 1, 1), _input(input) {};

    virtual std::string Type() const override { return max ? "ArgMax" : "ArgMin"; }

    // My outputs
    NodeOutput<ValueType> _val;
    NodeOutput<int> _argVal;

private:
    // My inputs
    NodeInput _input;
    
};

template <typename ValueType>
using ArgMinNode = ExtremalValNode<ValueType, false>;

template <typename ValueType>
using ArgMaxNode = ExtremalValNode<ValueType, true>;
