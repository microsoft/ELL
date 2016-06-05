#pragma once

//
// NodeOutput
// 

#include "NodeEdge.h"

#include <vector>
#include <memory>
#include <iostream>

class Node;

// TODO: maybe put type and size into NodeEdge and get rid of NodeOutputBase
class NodeOutputBase : public NodeEdge
{
public:


protected:
    NodeOutputBase(const class Node* node, size_t outputIndex, OutputType type, size_t size) : NodeEdge(node, outputIndex, type, size) 
    {
    };
};

template <typename ValueType>
class NodeOutput : public NodeOutputBase
{
public:
    NodeOutput(const class Node* node, size_t outputIndex, size_t size) : NodeOutputBase(node, outputIndex, NodeOutputBase::GetTypeCode<ValueType>(), size) {}

    void SetOutput(std::vector<ValueType> values) const { _cachedOutput = values; }
    std::vector<ValueType> GetOutput() const { return _cachedOutput; }
//    NodeOutput<ValueType> operator[](size_t index);

private:
    mutable std::vector<ValueType> _cachedOutput;
};

// template <typename ValueType>
// NodeOutput<ValueType> NodeOutput<ValueType>::operator[](size_t index)
// {
//     return NodeOutput<ValueType>(1, index);
// }
