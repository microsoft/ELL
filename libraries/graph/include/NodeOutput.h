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

    size_t GetSize() const { return _size; } // dimension
    OutputType GetType() const { return _type; }

protected:
    NodeOutputBase(const Node* node, size_t outputIndex, size_t size, OutputType type) : NodeEdge(node, outputIndex), _size(size), _type(type) 
    {
    };
    
private:
    size_t _size = 0;
    OutputType _type = OutputType::None;
};

template <typename ValueType>
class NodeOutput : public NodeOutputBase
{
public:
    NodeOutput(const Node* node, size_t outputIndex, size_t size) : NodeOutputBase(node, outputIndex, size, NodeOutputBase::GetTypeCode<ValueType>()) {}

//    NodeOutput<ValueType> operator[](size_t index);

private:
    std::vector<ValueType> _cachedOutput;
};

// template <typename ValueType>
// NodeOutput<ValueType> NodeOutput<ValueType>::operator[](size_t index)
// {
//     return NodeOutput<ValueType>(1, index);
// }
