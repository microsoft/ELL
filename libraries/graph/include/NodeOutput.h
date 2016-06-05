#pragma once

//
// NodeOutput
// 

#include "NodeEdge.h"

#include <vector>
#include <memory>
#include <iostream>

class Node;

//
// NodeOutputBase: base class of classes that represent outputs from a function (essentially identical to NodeEdge)
//
class NodeOutputBase : public NodeEdge
{
public:


protected:
    NodeOutputBase(const class Node* node, size_t outputIndex, OutputType type, size_t size) : NodeEdge(node, outputIndex, type, size) 
    {
    };
};

//
// NodeOutput<ValueType>: represents an output from a node/function
//
template <typename ValueType>
class NodeOutput : public NodeOutputBase
{
public:
    NodeOutput(const class Node* node, size_t outputIndex, size_t size) : NodeOutputBase(node, outputIndex, NodeOutputBase::GetTypeCode<ValueType>(), size) {}

    void SetOutput(std::vector<ValueType> values) const { _cachedOutput = values; }
    std::vector<ValueType> GetOutput() const { return _cachedOutput; }

private:
    mutable std::vector<ValueType> _cachedOutput;
};
