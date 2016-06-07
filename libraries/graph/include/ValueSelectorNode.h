#pragma once
//
// ValueSelectorNode
// 

#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

#include <vector>
#include <memory>
#include <exception>

template <typename ValueType>
class ValueSelectorNode : public Node
{
public:
    ValueSelectorNode(OutputPort<bool> condition, OutputPort<ValueType> value1, OutputPort<ValueType> value2);

    // Output
    const OutputPort<ValueType>& output = _output;

    // Returns the name for this node class
    static std::string GetTypeName() { return "ValueSelector"; }
    virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

protected:
    virtual void Compute() const override;

private:
    // Inputs
    InputPort _condition;
    InputPort _value1;
    InputPort _value2;
    
    // Output
    OutputPort<ValueType> _output;
};

#include "../tcc/ValueSelectorNode.tcc"
