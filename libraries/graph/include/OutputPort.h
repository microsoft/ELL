#pragma once

//
// OutputPort
// 

#include "Port.h"

#include <vector>
#include <memory>
#include <iostream>

class Node;

//
// OutputPortBase: base for classes that represent outputs from a function (essentially identical to Port)
//
class OutputPortBase : public Port
{
public:

protected:
    OutputPortBase(const class Node* node, size_t outputIndex, OutputType type, size_t size) : Port(node, outputIndex, type, size) 
    {
    };
};

//
// OutputPort<ValueType>: represents an output from a node/function
//
template <typename ValueType>
class OutputPort : public OutputPortBase
{
public:
    OutputPort(const class Node* node, size_t outputIndex, size_t size) : OutputPortBase(node, outputIndex, OutputPortBase::GetTypeCode<ValueType>(), size) {}

    void SetOutput(std::vector<ValueType> values) const { _cachedOutput = values; }
    std::vector<ValueType> GetOutput() const { return _cachedOutput; }

private:
    mutable std::vector<ValueType> _cachedOutput;
};
