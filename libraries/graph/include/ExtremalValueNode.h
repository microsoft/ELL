#pragma once
//
// ExtremalValNode
// 

#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

#include <vector>
#include <memory>
#include <algorithm>

template <typename ValueType, bool max>
class ExtremalValueNode: public Node
{
public:
    ExtremalValueNode(OutputPort<ValueType> input);

    // Returns the name for this node class
    static std::string GetTypeName() { return max ? "ArgMax" : "ArgMin"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    // read-only properties
    const OutputPort<ValueType>& val = _val;
    const OutputPort<int>& argVal = _argVal;

protected:
    virtual void Compute() const override;
    
private:
    // My inputs
    InputPort _input;
    
    // My outputs
    OutputPort<ValueType> _val;
    OutputPort<int> _argVal;
};

template <typename ValueType>
using ArgMinNode = ExtremalValueNode<ValueType, false>;

template <typename ValueType>
using ArgMaxNode = ExtremalValueNode<ValueType, true>;

#include "../tcc/ExtremalValueNode.tcc"
